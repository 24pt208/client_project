/**
 * @file main.cpp
 * @brief Главный модуль клиентского приложения
 */

#include <iostream>
#include <stdexcept>
#include "user_interface.h"
#include "file_handler.h"
#include "connection_manager.h"
#include <fstream>

/**
 * @brief Точка входа в программу
 * @param argc Количество аргументов
 * @param argv Массив аргументов
 * @return 0 при успехе, 1 при ошибке
 */
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
