#include "CommandServer.h"
#include <iostream>
#include <string_view>
#include <unordered_map>
#include "personDetector.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

CommandServer::CommandServer(PanTiltKit& kit, PersonDetector& detector) : panTiltKit(kit), personDetector(detector) {
    // Initialize Asio Transport
    wsServer.init_asio();

    // Set logging channels to only connection and disconnection
    wsServer.clear_access_channels(websocketpp::log::alevel::all);
    wsServer.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);

    // Register handler callbacks
    wsServer.set_open_handler(bind(&CommandServer::onOpen, this, ::_1));
    wsServer.set_close_handler(bind(&CommandServer::onClose, this, ::_1));
    wsServer.set_message_handler(bind(&CommandServer::onMessage, this, ::_1, ::_2));
}

void CommandServer::onOpen(const websocketpp::connection_hdl& hdl) {
    std::cout << "Client connected to Command Server.\n";
    wsConnections.insert(hdl);
}

void CommandServer::onClose(const websocketpp::connection_hdl& hdl) {
    std::cout << "Client disconnected from Command Server.\n";
    wsConnections.erase(hdl);
}

// Callback to listen and respond to commands
void CommandServer::onMessage(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg) {
    // All valid commands
    std::unordered_map<std::string_view, std::function<void()>> commandMap{
        {"count", [&] { getPeopleCount(hdl, msg); }},
        {"left", [&] { panTiltKit.increasePanAngle(); }},
        {"right", [&] { panTiltKit.decreasePanAngle(); }},
        {"up", [&] { panTiltKit.increaseTiltAngle(); }},
        {"down", [&] { panTiltKit.decreaseTiltAngle(); }},
        {"alarm", [&] { /* Alarm */ }},
        {"stop", [&] { /* Terminate */ }}
    };

    auto payload = msg->get_payload();
    std::cout << "Received command \"" << payload << "\" from " << hdl.lock().get() << std::endl;

    // Find and execute command
    auto it = commandMap.find(payload);
    if (it != commandMap.end()) {
        it->second();
    } else {
        std::cerr << "Unknown command found: " << payload << std::endl;
    }
}

void CommandServer::run(const uint16_t& port) {
    wsServer.listen(port);
    wsServer.start_accept();
    wsServer.run();
}

void CommandServer::getPeopleCount(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg) {
    try {
        wsServer.send(hdl, std::to_string(personDetector.getPeopleDetected()), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cerr << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
    }
}
