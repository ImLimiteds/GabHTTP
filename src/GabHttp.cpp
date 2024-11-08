#include "../include/GabHttp.hpp"

#include <sstream>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#endif

namespace GabHttp {
  std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::ranges::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) { return std::tolower(c); });
    return lower_str;
  }

  std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
      start++;
    }
    auto end = str.end();
    do {
      end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
      return std::string(start, end + 1);
  }

  Request::Request(const std::string & raw_request) {
    std::istringstream request_stream(raw_request);
    std::string line;

    if (std::getline(request_stream, line)) {
      std::istringstream request_line(line);
      request_line >> method >> path;
    }

    while (std::getline(request_stream, line) && line != "\r") {
      size_t delimiter_pos = line.find(": ");
      if (delimiter_pos != std::string::npos) {
        headers[line.substr(0, delimiter_pos)] = line.substr(delimiter_pos + 2);
      }
    }

    std::string body_line;
    while (std::getline(request_stream, body_line)) {
      body += body_line + "\n";
    }
  }

  std::string Response::to_string() const {
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << " OK\r\n";

    for (const auto & header: headers) {
      response << header.first << ": " << header.second << "\r\n";
    }

    response << "\r\n" << body;
    return response.str();
  }

  Server::Server(int port) {
    #ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), & wsaData);
    if (result != 0) {
      throw std::runtime_error("WSAStartup failed");
    }
    #endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
      throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(static_cast < uint16_t > (port));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, reinterpret_cast < sockaddr * > ( & server_addr), sizeof(server_addr)) < 0) {
      throw std::runtime_error("Failed to bind socket");
    }

    if (listen(server_socket, 10) < 0) {
      throw std::runtime_error("Failed to listen on socket");
    }

    std::cout << "Server listening on port " << port << std::endl;
  }

  Server::~Server() {
    Stop();
    #ifdef _WIN32
    closesocket(static_cast < SOCKET > (server_socket));
    WSACleanup();
    #else
    close(server_socket);
    #endif
  }

  void Server::Route(const std::string& method, const std::string& path, const std::function<void(Request&, Response&)>& handler, const std::map<std::string, std::string>& required_headers) {
    std::string method_upper = method;
    std::ranges::transform(method_upper.begin(), method_upper.end(), method_upper.begin(), ::toupper);
    routes[method_upper][path] = {handler, required_headers};
  }

  void Server::handle_client(int client_socket) {
    std::array<char, 4096> buffer{};

    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    getpeername(client_socket, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    #ifdef _WIN32
    int bytes_read = recv(client_socket, buffer.data(), buffer.size() - 1, 0);
    #else
    ssize_t bytes_read = recv(client_socket, buffer.data(), buffer.size() - 1, 0);
    #endif

    if (bytes_read <= 0) {
        #ifdef _WIN32
        closesocket(static_cast<SOCKET>(client_socket));
        #else
        close(client_socket);
        #endif
        return;
    }

    Request req(buffer.data());
    Response res;

    auto method_it = routes.find(req.method);
    if (method_it != routes.end()) {
        auto route_it = method_it->second.find(req.path);
        if (route_it != method_it->second.end()) {
            const auto& [handler, required_headers] = route_it->second;

            bool headers_valid = true;
            for (const auto& [key, value] : required_headers) {
                auto header_it = req.headers.find(key);
                if (header_it == req.headers.end() || trim(to_lower(header_it->second)) != trim(to_lower(value))) {
                    headers_valid = false;
                    break;
                }
            }

            if (!headers_valid) {
                res = Response(401);
                res.set_body("401 Unauthorized");
                res.set_header("Content-Type", "text/plain");
            } else {
                handler(req, res);
            }
        } else {
            res = Response(404);
            res.set_body("404 Not Found");
        }
    } else {
        res = Response(405);
        res.set_body("405 Method Not Allowed");
    }

    std::string response_str = res.to_string();
    send(client_socket, response_str.c_str(), response_str.size(), 0);
    std::time_t now = std::time(nullptr);
    char time[100];
    std::strftime(time, sizeof(time), "%d/%m/%Y, %H:%M:%S", std::localtime(&now));

    std::cout << "[" << time << "] - " << client_ip << " - " << req.method << " " << req.path << " -> " << res.get_status_code() << std::endl;

    #ifdef _WIN32
    closesocket(static_cast<SOCKET>(client_socket));
    #else
    close(client_socket);
    #endif
}

  void Server::start_worker() {
    while (running) {
      int client_socket = static_cast < int > (accept(server_socket, nullptr, nullptr));
      if (client_socket < 0) {
        if (running) {
          std::cerr << "Error accepting client connection\n";
        }
        continue;
      }

      std::thread client_thread( & Server::handle_client, this, client_socket);
      client_thread.detach();
    }
  }

  void Server::Start() {
    running = true;
    unsigned int thread_count = std::thread::hardware_concurrency();
    workers.reserve(thread_count);
    for (unsigned int i = 0; i < thread_count; ++i) {
      workers.emplace_back( & Server::start_worker, this);
    }
    for (auto & worker: workers) {
      worker.join();
    }
  }

  void Server::Stop() {
    running = false;
    #ifdef _WIN32
    closesocket(static_cast < SOCKET > (server_socket));
    #else
    shutdown(server_socket, SHUT_RDWR);
    close(server_socket);
    #endif
    for (auto & worker: workers) {
      if (worker.joinable()) {
        worker.join();
      }
    }
    workers.clear();
  }
}