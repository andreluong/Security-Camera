#define ASIO_STANDALONE
#define WEBSOCKETPP_CPP11_STL

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <opencv2/opencv.hpp>
#include <set>

typedef websocketpp::server<websocketpp::config::asio> server;

class BroadcastServer {
public:
    BroadcastServer();
    ~BroadcastServer() = default;

    void onOpen(const websocketpp::connection_hdl& hdl);
    void onClose(const websocketpp::connection_hdl& hdl);

    void run(const uint16_t& port);

    void sendFrame(const cv::Mat& frame);

private:
    typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connList;

    server wsServer;
    connList wsConnections;
};