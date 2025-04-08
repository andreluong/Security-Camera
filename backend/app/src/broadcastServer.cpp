#include "broadcastServer.h"
#include <iostream>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::bind;

BroadcastServer::BroadcastServer() : isRunning(true) {
    // Initialize Asio Transport
    wsServer.init_asio();

    // Set logging channels to only connection and disconnection
    wsServer.clear_access_channels(websocketpp::log::alevel::all);
    wsServer.set_access_channels(websocketpp::log::alevel::connect | websocketpp::log::alevel::disconnect);

    // Register handler callbacks
    wsServer.set_open_handler(bind(&BroadcastServer::onOpen, this, ::_1));
    wsServer.set_close_handler(bind(&BroadcastServer::onClose, this, ::_1));
    broadcastThread = std::thread(&BroadcastServer::run, this, 9002);
}

BroadcastServer::~BroadcastServer() {
    isRunning = false;
    wsServer.stop();
    if (broadcastThread.joinable()) broadcastThread.join();
    std::cout << "Broadcast server off" << std::endl;
}

void BroadcastServer::onOpen(const websocketpp::connection_hdl& hdl) {
    std::cout << "Client connected!\n";
    wsConnections.insert(hdl);
}

void BroadcastServer::onClose(const websocketpp::connection_hdl& hdl) {
    std::cout << "Client disconnected!\n";
    wsConnections.erase(hdl);
}

void BroadcastServer::run(const uint16_t& port) {
    while(isRunning) {
        wsServer.listen(port);
        wsServer.start_accept();
        wsServer.run();
    }
    std::cout << "Broadcast server stop" << std::endl;
}

// Send frame to all connections
void BroadcastServer::sendFrame(const cv::Mat& frame) {
    if (wsConnections.empty()) return;

    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);
    std::string encodedFrame(buf.begin(), buf.end());

    for (const auto& conn : wsConnections) {
        wsServer.send(conn, encodedFrame, websocketpp::frame::opcode::binary);
    }
}