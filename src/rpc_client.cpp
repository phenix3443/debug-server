#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::socket socket(io_context);
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 9000));

        // 发送数字
        std::string request = "5\n";
        boost::asio::write(socket, boost::asio::buffer(request));

        // 接收响应
        char buffer[1024] = {0};
        boost::system::error_code error;
        size_t length = socket.read_some(boost::asio::buffer(buffer), error);
        std::cout << "服务器返回：" << std::string(buffer, length) << std::endl;

    } catch (std::exception& e) {
        std::cerr << "客户端错误: " << e.what() << std::endl;
    }
    return 0;
}