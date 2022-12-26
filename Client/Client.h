#pragma once
#include <string_view>
#include <boost/asio.hpp>
#include <fstream>
#include "../Core/Core.h"

class Client{
public:
    Client(boost::asio::io_context& io_context);
    bool Connect(const ConnectionInfo& info);
    bool SendFile(const std::string& path_to_file);
    void Disconnect();
private:
    bool SendFileName(std::string_view path_to_file);
    bool WaitForRespond();
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver; 
};