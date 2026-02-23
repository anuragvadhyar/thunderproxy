#ifndef BACKEND_H
#define BACKEND_H
#include<iostream>
#include<string>

struct Backend {
    const std::string address;
    const std::string port;
    const std::string name;
    const std::string health_endpoint;
    unsigned int is_healthy;
    
    // Constructor to initialize const members
    Backend(const std::string& addr, const std::string& p, const std::string& n, 
            const std::string& health_endpoint, unsigned int healthy)
        : address(addr), port(p), name(n), health_endpoint(health_endpoint), is_healthy(healthy)
    {
    }
};
#endif
