#include <iostream>
#include <stdexcept>
#include "user_interface.h"
#include "file_handler.h"
#include "connection_manager.h"
#include <fstream>
#include <filesystem>

// Функция для создания демонстрационного входного файла
void createDemoInputFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Предупреждение: Не удалось создать файл " << filename << std::endl;
        return;
    }
    
    file << "3\n";           // количество векторов
    file << "4\n";           // размер первого вектора
    file << "1 2 3 4\n";     // значения первого вектора
    file << "3\n";           // размер второго вектора
    file << "10 20 30\n";    // значения второго вектора
    file << "2\n";           // размер третьего вектора
    file << "100 200\n";     // значения третьего вектора
    
    std::cout << "✓ Создан демонстрационный файл: " << filename << std::endl;
}

// Функция для создания конфигурационного файла
void createConfigFile(const std::string& filename) {
    // Обработка пути с ~
    std::string resolved_filename = filename;
    if (filename.find("~/") == 0) {
        const char* home = getenv("HOME");
        if (home) {
            resolved_filename = std::string(home) + filename.substr(1);
        }
    }
    
    // Создаем директорию если нужно
    std::filesystem::path config_path(resolved_filename);
    std::filesystem::create_directories(config_path.parent_path());
    
    std::ofstream file(resolved_filename);
    if (!file.is_open()) {
        std::cerr << "Предупреждение: Не удалось создать конфиг файл " << resolved_filename << std::endl;
        return;
    }
    
    file << "user:P@ssW0rd\n";
    std::cout << "✓ Создан конфигурационный файл: " << resolved_filename << std::endl;
}

// Функция для проверки и создания отсутствующих файлов
void ensureFilesExist(const std::string& input_file, const std::string& config_file) {
    // Проверяем и создаем входной файл
    if (!std::filesystem::exists(input_file)) {
        std::cout << "Входной файл не найден, создаем демонстрационный..." << std::endl;
        createDemoInputFile(input_file);
    }
    
    // Проверяем и создаем конфигурационный файл
    std::string resolved_config = config_file;
    if (config_file.find("~/") == 0) {
        const char* home = getenv("HOME");
        if (home) {
            resolved_config = std::string(home) + config_file.substr(1);
        }
    }
    
    if (!std::filesystem::exists(resolved_config)) {
        std::cout << "Конфигурационный файл не найден, создаем..." << std::endl;
        createConfigFile(config_file);
    }
}

int main(int argc, char** argv) {
    try {
        UserInterface ui;
        
        if (!ui.parseArguments(argc, argv)) {
            std::cout << ui.getHelp() << std::endl;
            return 0;
        }
        
        auto params = ui.getParams();
        
        std::cout << "=== Запуск клиента ===" << std::endl;
        std::cout << "Сервер: " << params.server_address << ":" << params.port << std::endl;
        std::cout << "Входной файл: " << params.input_file << std::endl;
        std::cout << "Выходной файл: " << params.output_file << std::endl;
        std::cout << "Конфигурация: " << params.config_file << std::endl;
        std::cout << std::endl;
        
        // Автоматически создаем недостающие файлы
        ensureFilesExist(params.input_file, params.config_file);
        std::cout << std::endl;
        
        std::cout << "1. Чтение конфигурации..." << std::endl;
        auto [login, password] = FileHandler::readConfig(params.config_file);
        
        std::cout << "2. Чтение векторов из файла..." << std::endl;
        auto vectors = FileHandler::readVectors(params.input_file);
        std::cout << "   Прочитано векторов: " << vectors.size() << std::endl;
        
        std::cout << "3. Подключение к серверу..." << std::endl;
        ConnectionManager conn(params.server_address, params.port);
        
        std::cout << "4. Аутентификация..." << std::endl;
        conn.authenticate(login, password);
        
        std::cout << "5. Обработка векторов на сервере..." << std::endl;
        auto results = conn.processVectors(vectors);
        
        std::cout << "6. Сохранение результатов..." << std::endl;
        FileHandler::saveResults(params.output_file, results);
        
        std::cout << std::endl;
        std::cout << "=== Успешно завершено ===" << std::endl;
        std::cout << "Обработано векторов: " << vectors.size() << std::endl;
        std::cout << "Результаты сохранены в: " << params.output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "=== ОШИБКА ===" << std::endl;
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
