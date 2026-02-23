#include "loadbalancer.h"

//Parse config file
json LoadBalancer::parseConfig()
{
    std::ifstream file(_config_file);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open config file: " + _config_file);
    }
    json data = json::parse(file);
    return data;
}

std::string LoadBalancer::parsePolicy() {
    if(data.contains("policy"))
    {
        return data["policy"].get<std::string>();
    }
    return "round-robin";  // Default if not in config
}

void LoadBalancer::parseBackends()
{
    if (!(data["servers"].size()))
    {
        throw std::runtime_error("Error: No backends registered with LB");
    }
    std::set<std::string> seen;
    for(auto& server : data["servers"])
    {
        std::string address = server["host"].get<std::string>() + ":" + server["port"].get<std::string>();
        if (seen.count(address) > 0){
            std::cerr<< "Error: Duplicate backend server provided in config!\n";
        }
        else{
            seen.insert(address);
        }
        backends.push_back(Backend(server["host"],
            server["port"],
            server["name"],
            server["health"],
            healthCount));

        Backend& b = backends.back();
        std::cout << "Loaded: " << b.name 
                    << " (" << b.address << ":" << b.port << ")" 
                    << std::endl;
    }
}

LoadBalancer::LoadBalancer(unsigned int port, const std::string config)
    : _port(port),
    _config_file(config),
    clientHandler(10),
    healthHandler(1),
    current_server(0),
    data(parseConfig()),
    policy(parsePolicy())
{
    pthread_mutex_init(&backends_mutex, nullptr);
}

void LoadBalancer::healthCheck(Backend &b)
{
    int statusCode;
    std::string contentType;
    std::string response = handleGetRequest(b.address, b.port, b.health_endpoint, statusCode);
    if (statusCode != 200)
    {
        if (b.is_healthy)
        {
            b.is_healthy--;
        }
        std::cerr << "Backend " << b.address << ":" << b.port << " is down with status " << statusCode << '\n';
        return;
    }
    if(!b.is_healthy)
    {
        b.is_healthy = healthCount;
    }

}

void LoadBalancer::configure()
{
    this->parseBackends();
}

void LoadBalancer::start_load_balancer()
{
    healthHandler.add_task([this]() 
        {
            while(1)
            {
                for (auto& b : this->backends) 
                {
                    this->healthCheck(b);
                }
                sleep(10);
            }
        }
    );
    //start health check
    this->healthHandler.start();
    //listen for client requests
    this->registerRoute();
    this->server.listen("0.0.0.0", this->_port);
}

void LoadBalancer::registerRoute()
{
    this->server.Get(".*", [this](const httplib::Request& req, httplib::Response& res) {
        Backend *b = this->SelectHostToForwardto();
        if(!b)
        {
            res.status = 503;  // Service Unavailable
            res.set_content("No healthy backends available", "text/plain");
            return;
        }
        int statusCode;
        std::string path = req.path;  // Use actual requested path
        std::string contentType;
        std::string response = handleGetRequest(b->address, b->port, path, statusCode);
        if (statusCode == 0)
        {
            res.status = 502;  // Bad Gateway
            res.set_content("Backend server unavailable", "text/plain");
            return;
        }
        res.status = statusCode;
        res.set_content(response, contentType);
    });
    return;
}

std::string LoadBalancer::handleGetRequest(const std::string& address, const unsigned int port, const std::string& path, int &statusCode)
{
    httplib::Client client(address, port);
    client.set_read_timeout(5, 0);
    client.set_connection_timeout(3, 0);
    auto res = client.Get(path);
    if(res)
    {
        statusCode = res->status;
        return res->body;
    }
    else
    {
        statusCode = 0;
        return "";
    }
}

Backend * LoadBalancer::SelectHostToForwardto()
{
    if(this->policy == "round-robin")
    {
        return RoundRobin(this->backends.size());
    }
    return NULL;
}

Backend * LoadBalancer::RoundRobin(size_t n)
{
    if(!n)
    {
        return nullptr;
    }
    pthread_mutex_lock(&backends_mutex);
    if(this->backends[this->current_server].is_healthy)
    {
        int temp{this->current_server};
        this->current_server = (this->current_server+1) % this->backends.size();
        pthread_mutex_unlock(&backends_mutex);
        return &this->backends[temp];
    }
    this->current_server = (this->current_server+1) % this->backends.size();
    pthread_mutex_unlock(&backends_mutex);
    return RoundRobin(n-1);
}