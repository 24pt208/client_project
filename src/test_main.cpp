/**
 * @file test_main.cpp
 * @author Пресняков А.А.
 * @version 1.0
 * @date 28.10.2025
 * @copyright ИБСТ ПГУ
 * @brief Модульные тесты для клиентского приложения
 * @details Содержит модульные тесты для всех компонентов системы:
 *          - UserInterface (пользовательский интерфейс)
 *          - FileHandler (работа с файлами)
 *          - AuthManager (аутентификация)
 *          - Интеграционные тесты
 * 
 * @note Использует фреймворк UnitTest++
 * @note Все тесты выполняются автоматически при запуске
 * 
 * @example
 * @code
 * // Сборка и запуск тестов
 * g++ -o test_client test_main.cpp user_interface.cpp file_handler.cpp \
 *      auth_manager.cpp -lUnitTest++ -lcryptopp
 * ./test_client
 * @endcode
 */

#include <UnitTest++/UnitTest++.h>
#include "user_interface.h"
#include "file_handler.h"
#include "auth_manager.h"
#include <fstream>
#include <cstdlib>
#include <vector>
#include <iostream>  

// Тестирование пользовательского интерфейса
SUITE(UserInterfaceTest)
{
    /**
     * @test HelpShort
     * @brief Тест короткой формы справки (-h)
     */
    TEST(HelpShort) {
        UserInterface ui;
        const char* argv[] = {"test", "-h", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!ui.parseArguments(argc, const_cast<char**>(argv)));
    }
    
    // ... остальные тесты UserInterface
}

// Фикстура для тестирования файлов
/**
 * @struct TestFileFixture
 * @brief Фикстура для тестирования работы с файлами
 * @details Создает временные файлы для тестирования
 *          и автоматически удаляет их после завершения тестов
 */
struct TestFileFixture {
    std::string test_filename;  ///< Имя тестового входного файла
    std::string test_output;    ///< Имя тестового выходного файла
    
    /**
     * @brief Конструктор фикстуры
     * @details Создает тестовый файл с векторами
     */
    TestFileFixture() : test_filename("test_input.txt"), test_output("test_output.bin") {
        // Создаем тестовый файл
        std::ofstream file(test_filename);
        file << "2\n";           // количество векторов
        file << "3\n";           // размер первого вектора
        file << "1 2 3\n";       // значения первого вектора
        file << "2\n";           // размер второго вектора  
        file << "10 20\n";       // значения второго вектора
        file.close();
    }
    
    /**
     * @brief Деструктор фикстуры
     * @details Удаляет все созданные временные файлы
     */
    ~TestFileFixture() {
        std::remove(test_filename.c_str());
        std::remove(test_output.c_str());
        std::remove("test_config.conf");
        std::remove("empty_file.txt");
        std::remove("invalid_file.txt");
    }
};

// Тестирование работы с файлами
SUITE(FileHandlerTest)
{
    /**
     * @test ReadVectorsValid
     * @brief Тест чтения корректного файла с векторами
     */
    TEST_FIXTURE(TestFileFixture, ReadVectorsValid) {
        auto vectors = FileHandler::readVectors("test_input.txt");
        CHECK_EQUAL(2, vectors.size());
        CHECK_EQUAL(3, vectors[0].size());
        CHECK_EQUAL(2, vectors[1].size());
        CHECK_EQUAL(1, vectors[0][0]);
        CHECK_EQUAL(2, vectors[0][1]);
        CHECK_EQUAL(3, vectors[0][2]);
        CHECK_EQUAL(10, vectors[1][0]);
        CHECK_EQUAL(20, vectors[1][1]);
    }
    
    // ... остальные тесты FileHandler
}

// Тестирование аутентификации
SUITE(AuthManagerTest)
{
    /**
     * @test GenerateSaltLength
     * @brief Тест длины генерируемой соли
     */
    TEST(GenerateSaltLength) {
        std::string salt = AuthManager::generateSalt();
        CHECK_EQUAL(16, salt.length()); // 64 бита = 16 hex цифр
    }
    
    // ... остальные тесты AuthManager
}

/**
 * @brief Главная функция тестов
 * @return Количество неудачных тестов (0 - все тесты прошли)
 * 
 * @details Запускает все тесты и выводит результаты
 */
int main() {
    std::cout << "=== Запуск модульных тестов клиента ===" << std::endl;
    std::cout << "Тестирование всех компонентов системы..." << std::endl;
    
    // Запуск всех тестов
    int result = UnitTest::RunAllTests();
    
    if (result == 0) {
        std::cout << "=== ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО ===" << std::endl;
    } else {
        std::cout << "=== НЕКОТОРЫЕ ТЕСТЫ НЕ ПРОЙДЕНЫ ===" << std::endl;
    }
    
    return result;
}
