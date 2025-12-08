/**
 * @file user_interface.h
 * @brief Заголовочный файл класса пользовательского интерфейса
 */

#pragma once
#include <string>
#include <boost/program_options.hpp>

/**
 * @struct ClientParams
 * @brief Структура для хранения параметров клиента
 */
struct ClientParams {
    std::string server_address; ///< Адрес сервера
    uint16_t port = 33333;      ///< Порт сервера
    std::string input_file;     ///< Входной файл
    std::string output_file;    ///< Выходной файл
    std::string config_file = "~/.config/vclient.conf"; ///< Файл конфигурации
};

/**
 * @class UserInterface
 * @brief Класс для обработки параметров командной строки
 */
class UserInterface {
public:
    UserInterface();
    bool parseArguments(int argc, char** argv);
    ClientParams getParams() const { return params_; }
    std::string getHelp() const;
    
private:
    boost::program_options::options_description desc_;
    ClientParams params_;
};
