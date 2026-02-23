#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include<iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "backend.h"
#include <set>
// #include <curl/curl.h>
// #include "http.h"
#include "thread_pool.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <httplib.h>

using json = nlohmann::json;
inline constexpr unsigned int healthCount{3};

class LoadBalancer {
    private:
        const unsigned int _port;
        const std::string _config_file;
        std::vector<Backend> backends;
        ThreadPool clientHandler;
        ThreadPool healthHandler;
        pthread_mutex_t backends_mutex;
        httplib::Server server;
        int current_server;
        const json data;
        const std::string policy;

        std::string parsePolicy();
        void parseBackends();
        Backend * RoundRobin(size_t n);
        json parseConfig();

    public:
        LoadBalancer(unsigned int port, const std::string config);
        void healthCheck(Backend &b);
        void configure();
        void start_load_balancer();
        void registerRoute();
        Backend * SelectHostToForwardto();
        std::string handleGetRequest(const std::string& address, const unsigned int port, const std::string& path, int& statusCode);
};

#endif