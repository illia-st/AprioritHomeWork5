#include "Client.h"

Client::Client(boost::asio::io_context& io_context):
    m_socket(io_context), m_resolver(io_context) {}
bool Client::Connect(const ConnectionInfo& info) {
    boost::system::error_code ec_resolve, ec_connect;
    boost::asio::connect(m_socket, m_resolver.resolve(info.ip, info.port, ec_resolve), ec_connect);
    if(ec_resolve == boost::asio::error::not_found || ec_connect == boost::asio::error::not_found){
        return false;
    }
    return true;
}

void Client::Disconnect(){
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close();
}
bool Client::SendFile(const std::string& path_to_file) {
    if(!SendFileName(path_to_file)){
        return false;
    }
    if(!WaitForRespond()) return false;
    // Now server is ready for the file content
    std::unique_ptr<FILE, int(*)(FILE*)> file(fopen(path_to_file.data(), "rb"), fclose);
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(Buffer_size);
    if(!file) return false;
    boost::system::error_code ec;
    try{
        while(true){
            std::size_t writen_bytes {fread(static_cast<void*>(buffer.get()), sizeof(*buffer.get()), Buffer_size, file.get())};
            if(!writen_bytes){
                if (ferror(file.get())) {
                    std::cerr << "Error reading file" << std::endl;
                }
                return false;
            }
            auto sent_bytes = m_socket.write_some(boost::asio::buffer(buffer.get(), writen_bytes));
            if(sent_bytes == 0) return false;
        }
    }
    catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        this->Disconnect();
        return false;
    }
    return true;
}

bool Client::SendFileName(std::string_view path_to_file){
    std::size_t start_index {};
#ifdef WIN32
    start_index = path_to_file.find_last_of('\\');
#else
    start_index = path_to_file.find_last_of('/');
#endif
    if(start_index == std::string_view::npos) start_index = 0;
    std::string_view file_name = path_to_file.substr(start_index + 1);
    boost::system::error_code ec;
    std::size_t offset {1};
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(Buffer_size);
    buffer.get()[0] = static_cast<uint8_t>(MsgType::FileName);
    std::for_each(file_name.begin(), file_name.end(), [&buffer, &offset](char smb){
        buffer.get()[offset++] = smb;
    });
    buffer.get()[offset] = '/';

    auto sent_bytes = boost::asio::write(m_socket, boost::asio::buffer(buffer.get(), Buffer_size));
    if(sent_bytes == 0) return false;
    return true;
}
bool Client::WaitForRespond(){
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(Buffer_size);
    auto received_bytes = boost::asio::read(m_socket, boost::asio::buffer(buffer.get(), Buffer_size));
    if(received_bytes == 0 || !buffer.get()[0]) return false;
    return true;
}
