/**
 * @file user_interface.cpp
 * @brief Реализация класса пользовательского интерфейса
 */

#include "user_interface.h"
#include <iostream>
#include <sstream>

UserInterface::UserInterface() : desc_("Допустимые параметры") {
    desc_.add_options()
        ("help,h", "Показать справку")
        ("server,s", boost::program_options::value<std::string>(&params_.server_address)->required(), "Адрес сервера")
        ("port,p", boost::program_options::value<uint16_t>(&params_.port)->default_value(33333), "Порт сервера")
        ("input,i", boost::program_options::value<std::string>(&params_.input_file)->required(), "Файл с исходными данными")
        ("output,o", boost::program_options::value<std::string>(&params_.output_file)->required(), "Файл для сохранения результатов")
        ("config,c", boost::program_options::value<std::string>(&params_.config_file)->default_value("~/.config/vclient.conf"), "Файл конфигурации");
}

bool UserInterface::parseArguments(int argc, char** argv) {
    namespace po = boost::program_options;
    
    try {
        po::variables_map vm;
        
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
                return false;
            }
        }
        
        if (argc == 1) {
            return false;
        }
        
        po::store(po::parse_command_line(argc, argv, desc_), vm);
        
        if (vm.count("help")) {
            return false;
        }
        
        bool has_required = vm.count("server") && vm.count("input") && vm.count("output");
        if (!has_required) {
            throw po::error("Отсутствуют обязательные параметры");
        }
        
        po::notify(vm);
        return true;
        
    } catch (const po::error& e) {
        throw std::runtime_error("Ошибка разбора параметров: " + std::string(e.what()) + "\nИспользуйте -h для справки");
    }
}

std::string UserInterface::getHelp() const {
    std::ostringstream oss;
    oss << "Клиент для обработки векторов на сервере\n\n";
    oss << "Использование:\n";
    oss << "  client -s АДРЕС_СЕРВЕРА -i ВХОДНОЙ_ФАЙЛ -o ВЫХОДНОЙ_ФАЙЛ [ОПЦИИ]\n\n";
    oss << desc_;
    oss << "\nПримеры:\n";
    oss << "  client -s 127.0.0.1 -i input.txt -o output.bin\n";
    return oss.str();
}
