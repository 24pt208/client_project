/**
 * @file connection_manager.cpp
 * @brief Реализация класса управления соединением
 */

#include "connection_manager.h"
#include "auth_manager.h"
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>

ConnectionManager::ConnectionManager(const std::string& server_addr, uint16_t port) 
    : socket_(-1), server_addr_(server_addr), port_(port) {
    
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == -1) {
        throw std::system_error(errno, std::generic_category(), "Ошибка создания сокета");
    }
    
    struct timeval timeout;
    timeout.tv_sec = 180;
    timeout.tv_usec = 0;
    
    if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(socket_);
        throw std::system_error(errno, std::generic_category(), "Ошибка установки таймаута приема");
    }
    
    if (setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(socket_);
        throw std::system_error(errno, std::generic_category(), "Ошибка установки таймаута отправки");
    }
    
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_);
    
    if (inet_pton(AF_INET, server_addr_.c_str(), &serv_addr.sin_addr) <= 0) {
        close(socket_);
        throw std::runtime_error("Неверный адрес сервера: " + server_addr_);
    }
    
    if (connect(socket_, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        close(socket_);
        throw std::system_error(errno, std::generic_category(), "Ошибка подключения к серверу " + server_addr_ + ":" + std::to_string(port_));
    }
}

ConnectionManager::~ConnectionManager() {
    if (socket_ != -1) {
        close(socket_);
    }
}

void ConnectionManager::authenticate(const std::string& login, const std::string& password) {
    std::string salt = AuthManager::generateSalt();
    std::string hash = AuthManager::computeHash(salt, password);
    
    std::string auth_message = login + salt + hash;
    sendString(auth_message);
    
    std::string response = receiveString(1024);
    
    if (response == "ERR") {
        throw std::runtime_error("Ошибка аутентификации. Сервер отклонил запрос");
    }
    
    if (response != "OK") {
        throw std::runtime_error("Неожиданный ответ сервера при аутентификации: " + response);
    }
}

std::vector<int32_t> ConnectionManager::processVectors(const std::vector<std::vector<int32_t>>& vectors) {
    std::vector<int32_t> results;
    results.reserve(vectors.size());
    
    uint32_t num_vectors = vectors.size();
    sendData(&num_vectors, sizeof(num_vectors));
    
    for (size_t i = 0; i < vectors.size(); ++i) {
        const auto& vector = vectors[i];
        
        uint32_t vector_size = vector.size();
        sendData(&vector_size, sizeof(vector_size));
        
        if (!vector.empty()) {
            sendData(vector.data(), vector_size * sizeof(int32_t));
        }
        
        int32_t result;
        receiveData(&result, sizeof(result));
        results.push_back(result);
    }
    
    return results;
}

void ConnectionManager::sendData(const void* data, size_t size) {
    ssize_t total_sent = 0;
    while (total_sent < static_cast<ssize_t>(size)) {
        ssize_t bytes_sent = send(socket_, 
                                 static_cast<const char*>(data) + total_sent, 
                                 size - total_sent, 0);
        if (bytes_sent == -1) {
            throw std::system_error(errno, std::generic_category(), "Ошибка отправки данных");
        }
        total_sent += bytes_sent;
    }
}

void ConnectionManager::receiveData(void* data, size_t size) {
    ssize_t total_received = 0;
    while (total_received < static_cast<ssize_t>(size)) {
        ssize_t bytes_received = recv(socket_, 
                                     static_cast<char*>(data) + total_received, 
                                     size - total_received, 0);
        if (bytes_received == -1) {
            throw std::system_error(errno, std::generic_category(), "Ошибка приема данных");
        }
        if (bytes_received == 0) {
            throw std::runtime_error("Соединение закрыто сервером");
        }
        total_received += bytes_received;
    }
}

void ConnectionManager::sendString(const std::string& str) {
    sendData(str.c_str(), str.length());
}

std::string ConnectionManager::receiveString(size_t max_length) {
    std::vector<char> buffer(max_length);
    ssize_t bytes_received = recv(socket_, buffer.data(), buffer.size(), 0);
    if (bytes_received == -1) {
        throw std::system_error(errno, std::generic_category(), "Ошибка приема строки");
    }
    if (bytes_received == 0) {
        throw std::runtime_error("Соединение закрыто сервером");
    }
    return std::string(buffer.data(), bytes_received);
}
