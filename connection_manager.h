#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class ConnectionManager {
public:
    ConnectionManager(const std::string& server_addr, uint16_t port);
    virtual ~ConnectionManager();
    
    void authenticate(const std::string& login, const std::string& password);
    std::vector<int32_t> processVectors(const std::vector<std::vector<int32_t>>& vectors);
    
protected:
    int socket_;                    
    std::string server_addr_;
    uint16_t port_;
    
    
    virtual void sendData(const void* data, size_t size);
    virtual void receiveData(void* data, size_t size);
    virtual void sendString(const std::string& str);
    virtual std::string receiveString(size_t max_length = 1024);
};
