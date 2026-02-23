# thunderproxy

A high-performance HTTP load balancer written in C++ with support for multiple backend servers and health checking.

## Features

- **Round-robin load balancing** across multiple backend servers
- **Health checking** - automatic detection of unhealthy backends
- **Thread pool** for concurrent request handling using `std::thread`
- **JSON configuration** for easy setup
- **HTTP forwarding** with status code propagation

## Architecture

- **Thread Pool**: Custom implementation using C++ standard library threading
- **HTTP Client**: libcurl for backend communication
- **HTTP Server**: cpp-httplib for handling incoming requests
- **JSON Parsing**: nlohmann/json for configuration

## Requirements

- C++17 compiler (g++, clang++)
- libcurl
- cpp-httplib (header-only, included via Homebrew)
- nlohmann/json (header-only, included via Homebrew)

## Installation

### macOS (Homebrew)
```bash
brew install curl
brew install nlohmann-json
```

### Build
```bash
make
```

Or manually:
```bash
g++ -std=c++17 -I/opt/homebrew/include -Iinclude \
    src/main.cpp src/loadbalancer.cpp src/http.cpp src/thread_pool.cpp \
    -lcurl -o loadbalancer
```

## Configuration

Edit `config/loadbalancer.json`:

```json
{
  "servers": [
    {
      "name": "server1",
      "host": "localhost",
      "port": "8080",
      "health": "/health"
    },
    {
      "name": "server2",
      "host": "localhost",
      "port": "8081",
      "health": "/health"
    }
  ],
  "policy": "round-robin"
}
```

## Usage

1. Start your backend servers on the configured ports
2. Run the load balancer:
```bash
./loadbalancer
```

3. Send requests to the load balancer:
```bash
curl http://localhost:80/
```

The load balancer will distribute requests across healthy backends using round-robin.

## Project Structure

```
.
├── README.md
├── Makefile
├── .gitignore
├── config/
│   └── loadbalancer.json      # Configuration file
├── include/
│   ├── backend.h              # Backend server structure
│   ├── http.h                 # HTTP client interface
│   ├── loadbalancer.h         # Load balancer class
│   └── thread_pool.h          # Thread pool class
└── src/
    ├── main.cpp               # Entry point
    ├── loadbalancer.cpp       # Load balancer implementation
    ├── http.cpp               # HTTP client implementation
    └── thread_pool.cpp        # Thread pool implementation
```

## How It Works

1. **Configuration Loading**: Parses JSON config to load backend servers
2. **Health Checking**: Separate thread pool periodically checks backend health endpoints
3. **Request Handling**: 
   - Incoming requests are received by cpp-httplib server
   - Load balancer selects a healthy backend using round-robin
   - Request is forwarded to the backend using libcurl
   - Response is returned to the client
4. **Thread Safety**: Mutex protection for shared backend state

## Improvements
1. Additional load balancing algorithms to be added
## License

MIT License
