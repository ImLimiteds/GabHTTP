#pragma once

#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <atomic>
#include <array>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace GabHttp {

    class Request {
    public:
        std::string method;
        std::string path;
        std::map<std::string, std::string> headers;
        std::string body;

        Request(const std::string& raw_request);
    };

    class Response {
    private:
        std::string body;
        int status_code;
        std::map<std::string, std::string> headers;

    public:
        Response(int status = 200) : status_code(status) {}

        void set_body(const std::string& content) { body = content; }
        void set_header(const std::string& key, const std::string& value) { headers[key] = value; }
        std::string to_string() const;

        int get_status_code() const {
            return status_code;
        }
    };

    class Server {
    private:
        int port;
        int server_socket;
        std::map<std::string, std::function<void(Request&, Response&)>> routes;
        std::vector<std::thread> workers;
        std::mutex client_mutex;
        std::atomic<bool> running;

        void handle_client(int client_socket);
        void start_worker();

    public:
        Server(int port);
        ~Server();
        void route(const std::string& path, const std::function<void(Request&, Response&)>& handler);
        void start();
        void stop();
    };

}