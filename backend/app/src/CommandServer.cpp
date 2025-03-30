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
    std::cout << hdl.lock().get() << " connected to Command Server.\n";
    wsConnections.insert(hdl);
}

void CommandServer::onClose(const websocketpp::connection_hdl& hdl) {
    std::cout << hdl.lock().get() << " disconnected from Command Server.\n";
    wsConnections.erase(hdl);
}

// Callback to listen and respond to commands
void CommandServer::onMessage(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg) {
    // All valid commands
    std::unordered_map<std::string, std::function<void()>> commandMap{
        {"count", [this, &hdl, &msg] { sendPeopleCount(hdl, msg); }},
        {"left", [this] { panTiltKit.increasePanAngle(); }},
        {"right", [this] { panTiltKit.decreasePanAngle(); }},
        {"up", [this] { panTiltKit.increaseTiltAngle(); }},
        {"down", [this] { panTiltKit.decreaseTiltAngle(); }},
        {"alarm", [] { /* Alarm */ }},
        {"stop", [this, &hdl] { terminate(hdl); }}
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

// TODO: Only disconnects client. Should close terminate program
// [2025-03-29 02:02:03] [disconnect] Disconnect close local:[1000,client exit] remote:[1000,client exit]
void CommandServer::terminate(const websocketpp::connection_hdl& hdl) {
    wsServer.close(hdl, websocketpp::close::status::normal, "client exit");
}

void CommandServer::sendPeopleCount(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg) {
    try {
        const std::string payload = "count: " + std::to_string(personDetector.getPeopleDetected());
        wsServer.send(hdl, payload, msg->get_opcode());
        std::cout << "Sent " << payload << std::endl;
    } catch (websocketpp::exception const & e) {
        std::cerr << "Echo failed because: " << "(" << e.what() << ")" << std::endl;
    }
}
