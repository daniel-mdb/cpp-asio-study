#include <functional>
#include <iostream>

#include <asio.hpp>
#include <cassert>

using asio::ip::tcp;

struct Session : public std::enable_shared_from_this<Session> {

  using Buffer = std::array<char, 1024>;

  ~Session() {
    std::cerr << __func__ << std::endl;
  }

  template<class T>
  Session(T io_context, tcp::socket && client) :
    client(std::move(client)), server(io_context) { }

  void proxy(tcp::endpoint target) {
    std::cerr << __func__ << std::endl;
    server.async_connect(target, std::bind_front(&Session::connect, shared_from_this()));
  }

  void connect(std::error_code ec) {
    std::cerr << __func__ << std::endl;
    if (ec) {
      // error
      std::cerr << "error " << ec.message() << std::endl;
      return;
    }
    transfer(client, server);
    transfer(server, client);
  }

  void transfer(tcp::socket & from, tcp::socket & to) {
    std::cerr << __func__ << std::endl;
    auto buffer = std::make_unique<Buffer>();
    auto bufferPtr = buffer.get();
    from.async_read_some(asio::buffer(*bufferPtr),
        std::bind_front(&Session::read, shared_from_this(), std::move(buffer), std::ref(from), std::ref(to)));
  }

  void read(std::unique_ptr<Buffer> && buffer, tcp::socket & from, tcp::socket & to, std::error_code ec, const std::size_t length) {
    std::cerr << __func__ << std::endl;
    if (ec) {
      std::cerr << "error " << ec.message() << std::endl;
      return;
    }
    auto bufferPtr = buffer.get();
    to.async_write_some(asio::buffer(*bufferPtr, length),
        std::bind_front(&Session::write, shared_from_this(), std::move(buffer)));
    transfer(from, to);
  }

  void write(std::unique_ptr<Buffer> &&, std::error_code ec, const std::size_t) {
    std::cerr << __func__ << std::endl;
    if (ec) {
      std::cerr << "error " << ec.message() << std::endl;
      return;
    }
  }

  tcp::socket client, server;
};

struct Proxy {
  using SessionShared = std::shared_ptr<Session>;
  using SessionWeak = std::weak_ptr<Session>;

  ~Proxy() {
    std::cerr << __func__ << std::endl;
  }

  Proxy(asio::io_context & io_context, const short port) :
    acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
      std::cerr << __func__ << std::endl;
      std::cout << "port " << port << std::endl;
      static const asio::socket_base::reuse_address reuse_address(true);
      acceptor.set_option(reuse_address);
      // acceptor.set_option(asio::ip::tcp::no_delay(true));
    }

  void listen(char * const target_host, char * const target_port) {
    try {
      target_endpoint = *tcp::resolver(acceptor.get_executor()).resolve(target_host, target_port);
    } catch (...) {
    }

    listen();
  }

  void listen(void) {
    std::cout << __func__ << std::endl;
    auto socket = std::make_unique<tcp::socket>(acceptor.get_executor());
    auto socketPtr = socket.get();
    acceptor.async_accept(*socketPtr, std::bind_front(&Proxy::accept, this, std::move(socket)));
  }

  void accept(std::unique_ptr<tcp::socket> && client, std::error_code ec) {
    std::cout << __func__ << std::endl;
    if (ec) {
      std::cerr << "error " << ec.message() << std::endl;
      return;
    };
    client->set_option(asio::ip::tcp::no_delay(true));
    auto session = std::make_shared<Session>(acceptor.get_executor(), std::move(*client));
    session->proxy(target_endpoint);
    sessions.emplace_back(session);

    // end by recursing into listen
    listen();
  }


  tcp::endpoint target_endpoint;
  tcp::acceptor acceptor;
  std::vector<SessionWeak> sessions;
};

int main(int argc, char * argv[]) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: " << argv[0]
        << " <target_address> <target_port> <listen_address>\n";
      return 1;
    }

    asio::io_context io_context;
    Proxy proxy(io_context, std::stoi(argv[3]));
    proxy.listen(argv[1], argv[2]);
    io_context.run();
    std::cout << "end" << std::endl;

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}

