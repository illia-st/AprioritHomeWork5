#pragma once
#include <string>
#include <iostream>

const std::string loopback = "127.0.0.1";
const uint16_t port = 12345;
const uint32_t Buffer_size = 1024;
struct ConnectionInfo {
    std::string ip;
    std::string port;
};
enum class MsgType: uint8_t {
    FileName = 1,
};



