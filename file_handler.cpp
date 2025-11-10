#include "file_handler.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

std::vector<std::vector<int32_t>> FileHandler::readVectors(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::system_error(errno, std::generic_category(), "Не удалось открыть файл: " + filename);
    }
    
    std::vector<std::vector<int32_t>> vectors;
    uint32_t num_vectors;
    
    file >> num_vectors;
    if (file.fail()) {
        throw std::runtime_error("Ошибка чтения количества векторов из файла: " + filename);
    }
    
    for (uint32_t i = 0; i < num_vectors; ++i) {
        uint32_t vector_size;
        file >> vector_size;
        if (file.fail()) {
            throw std::runtime_error("Ошибка чтения размера вектора " + std::to_string(i+1));
        }
        
        std::vector<int32_t> vector_data;
        vector_data.reserve(vector_size);
        
        for (uint32_t j = 0; j < vector_size; ++j) {
            int32_t value;
            file >> value;
            if (file.fail()) {
                throw std::runtime_error("Ошибка чтения значения вектора " + std::to_string(i+1) + 
                                       ", элемент " + std::to_string(j+1));
            }
            vector_data.push_back(value);
        }
        
        vectors.push_back(std::move(vector_data));
    }
    
    return vectors;
}

void FileHandler::saveResults(const std::string& filename, const std::vector<int32_t>& results) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::system_error(errno, std::generic_category(), "Не удалось создать файл: " + filename);
    }
    
    uint32_t num_results = results.size();
    file.write(reinterpret_cast<const char*>(&num_results), sizeof(num_results));
    if (file.fail()) {
        throw std::runtime_error("Ошибка записи количества результатов в файл: " + filename);
    }
    
    for (const auto& result : results) {
        file.write(reinterpret_cast<const char*>(&result), sizeof(result));
        if (file.fail()) {
            throw std::runtime_error("Ошибка записи результата в файл: " + filename);
        }
    }
}

std::pair<std::string, std::string> FileHandler::readConfig(const std::string& filename) {
    std::string resolved_filename = filename;
    
    if (filename.find("~/") == 0) {
        const char* home = getenv("HOME");
        if (home) {
            resolved_filename = std::string(home) + filename.substr(1);
        }
    }
    
    std::ifstream file(resolved_filename);
    if (!file.is_open()) {
        throw std::system_error(errno, std::generic_category(), "Не удалось открыть файл конфигурации: " + resolved_filename);
    }
    
    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error("Файл конфигурации пуст: " + resolved_filename);
    }
    
    size_t colon_pos = line.find(':');
    if (colon_pos == std::string::npos) {
        throw std::runtime_error("Неверный формат файла конфигурации. Ожидается: login:password");
    }
    
    std::string login = line.substr(0, colon_pos);
    std::string password = line.substr(colon_pos + 1);
    
    if (login.empty() || password.empty()) {
        throw std::runtime_error("Логин или пароль не могут быть пустыми в файле конфигурации");
    }
    
    return {login, password};
}
