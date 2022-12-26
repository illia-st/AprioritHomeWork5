#include <iostream>
#include "Server.h"

int main(int argc, const char**argv){
    try {
        if(argc < 2) {
            std::cerr << "Usage: <host>" << std::endl;
            return -1;
        }
        boost::asio::io_context context;
        Server _server(context, std::stoi(argv[1]));
        context.run();
    } catch(const std::exception& ex){
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}
