#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// 计算平方的函数
int square(int num) {
    return num * num;
}

// 获取输入数据（本地输入或网络）
int get_input(bool use_network, tcp::socket* socket = nullptr) {
    int number;
    if (use_network && socket) {
        try {
            char buffer[1024] = {0};
            boost::system::error_code error;
            size_t length = socket->read_some(boost::asio::buffer(buffer), error);

            if (error == boost::asio::error::eof) {
                std::cerr << "客户端断开连接" << std::endl;
                return -1;
            } else if (error) {
                throw boost::system::system_error(error);
            }

            number = std::stoi(std::string(buffer, length));
            std::cout << "收到数字：" << number << std::endl;
        } catch (std::exception& e) {
            std::cerr << "输入错误: " << e.what() << std::endl;
            return -1;
        }
    } else {
        std::cout << "请输入一个整数（输入 0 退出）: ";
        std::cin >> number;
        if (std::cin.fail()) {
            std::cerr << "输入无效，请输入整数。" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return -1;
        }
    }
    return number;
}

// 发送输出（本地或网络）
void send_output(bool use_network, int result, tcp::socket* socket = nullptr) {
    std::string response = "平方：" + std::to_string(result) + "\n";

    if (use_network && socket) {
        boost::asio::write(*socket, boost::asio::buffer(response));
        std::cout << "返回结果：" << response;
    } else {
        std::cout << response;
    }
}

int main(int argc, char* argv[]) {
    bool use_network = (argc > 1 && std::string(argv[1]) == "--network");

    if (use_network) {
        try {
            boost::asio::io_context io_context;
            tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 9000));

            std::cout << "RPC 服务器（网络模式）已启动，监听端口 9000..." << std::endl;

            while (true) {
                tcp::socket socket(io_context);
                acceptor.accept(socket);

                int number = get_input(use_network, &socket);
                if (number == -1 || number == 0) break;

                int result = square(number);
                send_output(use_network, result, &socket);
            }
        } catch (std::exception& e) {
            std::cerr << "服务器错误: " << e.what() << std::endl;
        }
    } else {
        while (true) {
            int number = get_input(use_network);
            if (number == -1 || number == 0) break;

            int result = square(number);
            send_output(use_network, result);
        }
    }

    return 0;
}