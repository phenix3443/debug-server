#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <optional>
#include <chrono>
#include <atomic>

using boost::asio::ip::tcp;

struct Server {
    boost::asio::io_context io_context;
    std::optional<tcp::acceptor> acceptor;
    std::optional<tcp::socket> socket;
    std::thread network_thread;
    std::atomic<bool> running{true};

    void start_network_listener(int port) {
        acceptor.emplace(io_context, tcp::endpoint(tcp::v4(), port));
        std::cout << "RPC 服务器（网络模式）已启动，监听端口 " << port << "..." << std::endl;

        network_thread = std::thread([this]() {
            io_context.run();
        });
    }

    void stop_network_listener() {
        if (acceptor) {
            io_context.stop();
            if (network_thread.joinable()) {
                network_thread.join();
            }
            std::cout << "RPC 服务器已停止" << std::endl;
        }
    }

    std::string get_input(bool use_network) {
        std::string input;

        if (use_network && acceptor) {
            try {
                socket.emplace(io_context);
                acceptor->accept(*socket);

                char buffer[1024] = {0};
                boost::system::error_code error;
                size_t length = socket->read_some(boost::asio::buffer(buffer), error);
                if (error) throw boost::system::system_error(error);

                input = std::string(buffer, length);
                input.erase(input.find_last_not_of(" \r\n") + 1);
            } catch (std::exception& e) {
                std::cerr << "输入错误: " << e.what() << std::endl;
                return "";
            }
        } else {
            std::cout << "请输入: ";
            std::getline(std::cin, input);
            if (std::cin.fail()) {
                std::cerr << "输入无效，请重新输入。" << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return "";
            }
        }

        return input;
    }

    void send_output(bool use_network, const std::string& response) {
        if (use_network && socket) {
            boost::asio::write(*socket, boost::asio::buffer(response));
            std::cout << "返回结果：" << response << std::endl;
        } else {
            std::cout << response << std::endl;
        }
    }

    void start(bool use_network) {
        if (use_network && !acceptor) {
            std::cerr << "错误: 请先调用 start_network_listener() 初始化 acceptor!" << std::endl;
            return;
        }

        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::string input = get_input(use_network);
            if (input.empty() || input == "0") {
                running = false;
                break;
            }

            int number = std::stoi(input);
            int result = number * number;
            send_output(use_network, "平方：" + std::to_string(result));
        }

        stop_network_listener();
    }
};

int main(int argc, char* argv[]) {
    bool use_network = (argc > 1 && std::string(argv[1]) == "--network");

    Server server;
    if (use_network) {
        server.start_network_listener(9000);
    } else {
        std::cout << "没有启动远程调试" << std::endl;
    }

    server.start(use_network);
    return 0;
}