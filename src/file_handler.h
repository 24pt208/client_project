/**
 * @file file_handler.h
 * @brief Заголовочный файл класса работы с файлами
 */

#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <cstdint>

/**
 * @class FileHandler
 * @brief Статический класс для операций с файлами
 */
class FileHandler {
public:
    static std::vector<std::vector<int32_t>> readVectors(const std::string& filename);
    static void saveResults(const std::string& filename, const std::vector<int32_t>& results);
    static std::pair<std::string, std::string> readConfig(const std::string& filename);
    
private:
    FileHandler() = delete;
    ~FileHandler() = delete;
    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
};
