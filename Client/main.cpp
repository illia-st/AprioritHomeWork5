#include <iostream>
#include "Client.h"

int main(int argc, const char**argv){
    try{
        if(argc < 4) {
            std::cerr << "Usage: <host> <port> <file_to_transfer>" << std::endl;
            return 1;
        }
        boost::asio::io_context context;
        Client client(context);
        if(client.Connect(ConnectionInfo(argv[1], argv[2]))){
            client.SendFile(argv[3]);
            client.Disconnect();
        }
    } catch(const std::exception& ex){
        std::cerr << ex.what() << std::endl;
    }catch(...){
        std::cerr << "An unknown exception was caught" << std::endl;
    }
    return 0;
}