/**
 * @file auth_manager.h
 * @brief Заголовочный файл класса аутентификации
 */

#pragma once
#include <string>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

/**
 * @class AuthManager
 * @brief Статический класс для криптографических операций
 */
class AuthManager {
public:
    static std::string generateSalt();
    static std::string computeHash(const std::string& salt, const std::string& password);
    
private:
    AuthManager() = delete;
    ~AuthManager() = delete;
};
