#pragma once

#define ASIO_STANDALONE
#define WEBSOCKETPP_CPP11_STL

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>
#include "PanTiltKit.h"
#include "cameraFeed.h"
#include "personDetector.h"
#include <thread>
#include <atomic>
#include "Alarm.h"

/**
 * Commands:
 * 
 * - "toggle": Toggles between original & processed view
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
    CommandServer(PanTiltKit& kit, PersonDetector& detector, CameraFeed& feed, Alarm& a);
    ~CommandServer();

    void onOpen(const websocketpp::connection_hdl& hdl);
    void onClose(const websocketpp::connection_hdl& hdl);
    void onMessage(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg);

    void run(const uint16_t& port);

private:
    typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connList;
    
    PanTiltKit& panTiltKit;
    PersonDetector& personDetector;
    CameraFeed& cameraFeed;
    Alarm& alarm;
    server wsServer;
    connList wsConnections;

    std::atomic<bool> isRunning;
    std::thread commandThread;

    void terminate(const websocketpp::connection_hdl& hdl);
    void sendPeopleCount(const websocketpp::connection_hdl& hdl, const server::message_ptr& msg);
};