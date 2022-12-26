#include "Server.h"


// Session
Session::Session(tcp::socket socket)
    : socket_(std::move(socket)), data_(std::make_shared<char[]>(Buffer_size))
{ }

void Session::start(){
    Process();
}
void Session::Disconnect(){
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close();
}
void Session::Process()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_.get(), Buffer_size),
        [this, self](boost::system::error_code ec, std::size_t bytes_transfered){
        if(!ec){
            HandleRead(bytes_transfered, ec);
        }
    });
}
void Session::HandleRead(std::size_t bytes_transfered, boost::system::error_code& ec){
    std::cerr << "Log: ";
    switch (static_cast<MsgType>(data_.get()[0])) {
        case MsgType::FileName:
            // fill in file name
            if(is_file_created){
                std::cerr << "Wriring to file" << std::endl;
                if(!WriteToFile(bytes_transfered, ec)) return;
                break;
            }
            std::cerr << "Creating a file ";;
            for(size_t i {1}; data_.get()[i] != '/'; ++i){
                file_name.push_back(data_.get()[i]);
            }
            std::cerr << file_name << std::endl;
            RespondOnFileCreation(CreateFile(ec));
            break;
        default:
            std::cerr << "Wriring to file" << std::endl;
            if(!WriteToFile(bytes_transfered, ec)) return;
            break;
    }
    Process();
}
uint8_t Session::CreateFile(boost::system::error_code& ec){
    // check file closing
    std::unique_ptr<FILE, int(*)(FILE*)> file(fopen(file_name.data(), "wb"), fclose);
    if(!file){
        ec.assign(-1, boost::system::generic_category());
        return 0;
    }else{
        is_file_created = true;
        return 1;
    }
}
bool Session::WriteToFile(std::size_t length, boost::system::error_code& ec){
    // begin writing to file
    std::unique_ptr<FILE, int(*)(FILE*)> file(fopen(file_name.data(), "ab"), fclose);
    if(!file){
        ec.assign(-1, boost::system::generic_category());
        Disconnect();
        return false;
    }
    auto writen_bytes = fwrite(data_.get(), sizeof(*data_.get()), length, file.get());
    if(writen_bytes < length){
        ec.assign(-1, boost::system::generic_category());
        Disconnect();
        return false;
    }
    return true;
}
void Session::RespondOnFileCreation(uint8_t answer){
    auto self(shared_from_this());
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(Buffer_size);
    buffer.get()[0] = answer;
    boost::asio::write(socket_, boost::asio::buffer(buffer.get(), Buffer_size));
    if(!answer) Disconnect(); 
}
// Server
Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
}
void Server::do_accept(){
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket){
        if (!ec){
            std::cerr << "New connection from: " << socket.remote_endpoint().address() << ":" << socket.remote_endpoint().port() << std::endl;
            std::make_shared<Session>(std::move(socket))->start();
        }
        do_accept();
    });
}