#include "user_interface.h"
#include <iostream>
#include <sstream>

UserInterface::UserInterface() : desc_("Допустимые параметры") {
    desc_.add_options()
        ("help,h", "Показать справку")
        ("server,s", boost::program_options::value<std::string>(&params_.server_address), 
         "Адрес сервера (обязательный)")
        ("port,p", boost::program_options::value<uint16_t>(&params_.port)->default_value(33333), 
         "Порт сервера (по умолчанию: 33333)")
        ("input,i", boost::program_options::value<std::string>(&params_.input_file), 
         "Файл с исходными данными (обязательный)")
        ("output,o", boost::program_options::value<std::string>(&params_.output_file), 
         "Файл для сохранения результатов (обязательный)")
        ("config,c", boost::program_options::value<std::string>(&params_.config_file)->default_value("~/.config/vclient.conf"), 
         "Файл конфигурации с логином и паролем (по умолчанию: ~/.config/vclient.conf)");
}

bool UserInterface::parseArguments(int argc, char** argv) {
    namespace po = boost::program_options;
    
    try {
        po::variables_map vm;
        
        // Сначала проверяем наличие -h без полного разбора
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
                return false;
            }
        }
        
        po::store(po::parse_command_line(argc, argv, desc_), vm);
        
        // Если нет параметров - показываем справку
        if (argc == 1) {
            return false;
        }
        
        po::notify(vm);
        return true;
        
    } catch (const po::error& e) {
        throw std::runtime_error("Ошибка разбора параметров: " + std::string(e.what()) + 
                               "\nИспользуйте -h для справки");
    }
}

std::string UserInterface::getHelp() const {
    std::ostringstream oss;
    oss << "Клиент для обработки векторов на сервере\n\n";
    oss << "Использование:\n";
    oss << "  client -s АДРЕС_СЕРВЕРА -i ВХОДНОЙ_ФАЙЛ -o ВЫХОДНОЙ_ФАЙЛ [ОПЦИИ]\n\n";
    oss << desc_;
    oss << "\nОбязательные параметры:\n";
    oss << "  -s, --server АДРЕС    IP-адрес или доменное имя сервера\n";
    oss << "  -i, --input ФАЙЛ      Текстовый файл с векторами для обработки\n";
    oss << "  -o, --output ФАЙЛ     Двоичный файл для сохранения результатов\n";
    oss << "\nПримеры:\n";
    oss << "  client -s 127.0.0.1 -i input.txt -o output.bin\n";
    oss << "  client --server 192.168.1.1 --port 4444 --input data.txt --output result.bin\n";
    oss << "  client --config my.conf -s localhost -i vectors.txt -o results.bin\n";
    oss << "\nФормат входного файла:\n";
    oss << "  Первая строка: количество векторов (целое число)\n";
    oss << "  Для каждого вектора:\n";
    oss << "    - размер вектора (целое число)\n";
    oss << "    - значения вектора (целые числа, разделенные пробелами)\n";
    oss << "\nФормат конфигурационного файла:\n";
    oss << "  Одна строка в формате: логин:пароль\n";
    oss << "  Пример: user:P@ssW0rd\n";
    return oss.str();
}
