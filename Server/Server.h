#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "../Core/Core.h"

using boost::asio::ip::tcp;

class Session
: public std::enable_shared_from_this<Session>
{
public:
  Session(tcp::socket socket);
  void start();
  void Disconnect();
private:
  void Process();
  void RespondOnFileCreation(uint8_t answer);
  void HandleRead(std::size_t bytes_transfered, boost::system::error_code& ec);
  uint8_t CreateFile(boost::system::error_code& ec);
  bool WriteToFile(std::size_t length, boost::system::error_code& ec);
  tcp::socket socket_;
  std::shared_ptr<char[]> data_;
  std::string file_name {};
  bool is_file_created {false};
};

class Server
{
public:
  Server(boost::asio::io_context& io_context, short port);

private:
  void do_accept();
  tcp::acceptor acceptor_;
};