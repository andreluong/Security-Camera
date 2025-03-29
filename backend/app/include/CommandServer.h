#pragma once

#define ASIO_STANDALONE
#define WEBSOCKETPP_CPP11_STL

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include "PanTiltKit.h"
#include "personDetector.h"

/**
 * Commands:
 * 
 * - "count": Receives people count
 * - "left": Moves pan left
 * - "right": Moves pan right
 * - "up": Moves tilt up
 * - "down": Moves tilt down
 * - "alarm": Triggers alarm
 * - "stop": Terminates program
 */

typedef websocketpp::server<websocketpp::config::asio> server;

class CommandServer {
public:
    CommandServer(PanTiltKit& kit, PersonDetector& detector);
    ~CommandServer() = default;

    void onOpen(const websocketpp::connection_hdl& hdl);
    void onClose(const websocketpp::connection_hdl& hdl);
    void onMessage(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg);

    void run(const uint16_t& port);

private:
    typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connList;
    
    PanTiltKit& panTiltKit;
    PersonDetector& personDetector;
    server wsServer;
    connList wsConnections;

    void getPeopleCount(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg);
};