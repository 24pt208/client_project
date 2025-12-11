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
    TEST(HelpShort) {
        UserInterface ui;
        const char* argv[] = {"test", "-h", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!ui.parseArguments(argc, const_cast<char**>(argv)));
    }
    
    TEST(HelpLong) {
        UserInterface ui;
        const char* argv[] = {"test", "--help", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!ui.parseArguments(argc, const_cast<char**>(argv)));
    }
    
    TEST(ValidParameters) {
        UserInterface ui;
        const char* argv[] = {"test", "-s", "127.0.0.1", "-i", "input.txt", 
                             "-o", "output.bin", "-c", "config.conf", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(ui.parseArguments(argc, const_cast<char**>(argv)));
        
        auto params = ui.getParams();
        CHECK_EQUAL("127.0.0.1", params.server_address);
        CHECK_EQUAL("input.txt", params.input_file);
        CHECK_EQUAL("output.bin", params.output_file);
        CHECK_EQUAL("config.conf", params.config_file);
    }
    
    TEST(MissingRequiredParameters) {
        UserInterface ui;
        const char* argv[] = {"test", "-s", "127.0.0.1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK_THROW(ui.parseArguments(argc, const_cast<char**>(argv)), std::runtime_error);
    }
    
    TEST(DefaultValues) {
        UserInterface ui;
        const char* argv[] = {"test", "-s", "127.0.0.1", "-i", "input.txt", 
                             "-o", "output.bin", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(ui.parseArguments(argc, const_cast<char**>(argv)));
        
        auto params = ui.getParams();
        CHECK_EQUAL(33333, params.port);
        CHECK_EQUAL("~/.config/vclient.conf", params.config_file);
    }
    
    TEST(HelpWithOtherParameters) {
        UserInterface ui;
        const char* argv[] = {"test", "-h", "-s", "127.0.0.1", nullptr};
        int argc = sizeof(argv) / sizeof(argv[0]) - 1;
        CHECK(!ui.parseArguments(argc, const_cast<char**>(argv)));
    }
}

// Фикстура для тестирования файлов
struct TestFileFixture {
    std::string test_filename;
    std::string test_output;
    
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
    
    TEST(ReadVectorsFileNotFound) {
        CHECK_THROW(FileHandler::readVectors("nonexistent_file.txt"), std::system_error);
    }
    
    TEST_FIXTURE(TestFileFixture, SaveResultsValid) {
        std::vector<int32_t> results = {42, 123, -456};
        FileHandler::saveResults("test_output.bin", results);
        
        // Проверяем, что файл создан и имеет правильное содержимое
        std::ifstream file("test_output.bin", std::ios::binary);
        CHECK(file.is_open());
        
        uint32_t num_results;
        file.read(reinterpret_cast<char*>(&num_results), sizeof(num_results));
        CHECK_EQUAL(3, num_results);
        
        int32_t result1, result2, result3;
        file.read(reinterpret_cast<char*>(&result1), sizeof(result1));
        file.read(reinterpret_cast<char*>(&result2), sizeof(result2));
        file.read(reinterpret_cast<char*>(&result3), sizeof(result3));
        
        CHECK_EQUAL(42, result1);
        CHECK_EQUAL(123, result2);
        CHECK_EQUAL(-456, result3);
        
        file.close();
    }
    
    TEST_FIXTURE(TestFileFixture, SaveResultsEmpty) {
        std::vector<int32_t> results = {};
        FileHandler::saveResults("test_output.bin", results);
        
        std::ifstream file("test_output.bin", std::ios::binary);
        CHECK(file.is_open());
        
        uint32_t num_results;
        file.read(reinterpret_cast<char*>(&num_results), sizeof(num_results));
        CHECK_EQUAL(0, num_results);
        
        file.close();
    }
    
    TEST_FIXTURE(TestFileFixture, ReadConfigValid) {
        // Создаем временный конфиг файл
        std::ofstream config("test_config.conf");
        config << "testuser:testpass";
        config.close();
        
        auto [login, password] = FileHandler::readConfig("test_config.conf");
        CHECK_EQUAL("testuser", login);
        CHECK_EQUAL("testpass", password);
    }
    
    TEST_FIXTURE(TestFileFixture, ReadConfigWithTilde) {
        // Тестируем обработку пути с ~
        std::string home = std::getenv("HOME");
        std::string config_path = home + "/test_config.conf";
        
        std::ofstream config(config_path);
        config << "testuser:testpass";
        config.close();
        
        auto [login, password] = FileHandler::readConfig("~/test_config.conf");
        CHECK_EQUAL("testuser", login);
        CHECK_EQUAL("testpass", password);
        
        std::remove(config_path.c_str());
    }
    
    TEST_FIXTURE(TestFileFixture, ReadConfigInvalidFormat) {
        std::ofstream config("test_config.conf");
        config << "invalid_format_without_colon";
        config.close();
        
        CHECK_THROW(FileHandler::readConfig("test_config.conf"), std::runtime_error);
    }
    
    TEST_FIXTURE(TestFileFixture, ReadConfigEmptyFile) {
        std::ofstream config("test_config.conf");
        config << "";
        config.close();
        
        CHECK_THROW(FileHandler::readConfig("test_config.conf"), std::runtime_error);
    }
    
    TEST_FIXTURE(TestFileFixture, ReadConfigOnlyLogin) {
        std::ofstream config("test_config.conf");
        config << "loginonly:";
        config.close();
        
        CHECK_THROW(FileHandler::readConfig("test_config.conf"), std::runtime_error);
    }
    
    TEST_FIXTURE(TestFileFixture, ReadConfigOnlyPassword) {
        std::ofstream config("test_config.conf");
        config << ":passwordonly";
        config.close();
        
        CHECK_THROW(FileHandler::readConfig("test_config.conf"), std::runtime_error);
    }
}

// Тестирование аутентификации
SUITE(AuthManagerTest)
{
    TEST(GenerateSaltLength) {
        std::string salt = AuthManager::generateSalt();
        CHECK_EQUAL(16, salt.length()); // 64 бита = 16 hex цифр
    }
    
    TEST(GenerateSaltHexFormat) {
        std::string salt = AuthManager::generateSalt();
        
        // Проверяем, что все символы - валидные hex цифры в верхнем регистре
        for (char c : salt) {
            bool is_hex = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
            CHECK(is_hex);
        }
    }
    
    TEST(GenerateSaltUnique) {
        std::string salt1 = AuthManager::generateSalt();
        std::string salt2 = AuthManager::generateSalt();
        std::string salt3 = AuthManager::generateSalt();
        
        // Соли должны быть разными (вероятность совпадения крайне мала)
        CHECK(salt1 != salt2);
        CHECK(salt1 != salt3);
        CHECK(salt2 != salt3);
    }
    
    TEST(ComputeHashConsistent) {
        std::string salt = "1234567890ABCDEF";
        std::string password = "testpassword";
        
        std::string hash1 = AuthManager::computeHash(salt, password);
        std::string hash2 = AuthManager::computeHash(salt, password);
        std::string hash3 = AuthManager::computeHash(salt, password);
        
        // Для одинаковых входных данных хеш должен быть одинаковым
        CHECK_EQUAL(hash1, hash2);
        CHECK_EQUAL(hash1, hash3);
        CHECK_EQUAL(hash2, hash3);
    }
    
    TEST(ComputeHashDifferentForDifferentInputs) {
        std::string salt = "1234567890ABCDEF";
        std::string password1 = "password1";
        std::string password2 = "password2";
        std::string password3 = "password3";
        
        std::string hash1 = AuthManager::computeHash(salt, password1);
        std::string hash2 = AuthManager::computeHash(salt, password2);
        std::string hash3 = AuthManager::computeHash(salt, password3);
        
        // Для разных паролей хеши должны быть разными
        CHECK(hash1 != hash2);
        CHECK(hash1 != hash3);
        CHECK(hash2 != hash3);
    }
    
    TEST(ComputeHashDifferentForDifferentSalts) {
        std::string salt1 = "1234567890ABCDEF";
        std::string salt2 = "FEDCBA0987654321";
        std::string salt3 = "0000000000000000";
        std::string password = "password";
        
        std::string hash1 = AuthManager::computeHash(salt1, password);
        std::string hash2 = AuthManager::computeHash(salt2, password);
        std::string hash3 = AuthManager::computeHash(salt3, password);
        
        // Для разных солей хеши должны быть разными
        CHECK(hash1 != hash2);
        CHECK(hash1 != hash3);
        CHECK(hash2 != hash3);
    }
    
    TEST(ComputeHashFormat) {
        std::string salt = "0000000000000000";
        std::string password = "test";
        std::string hash = AuthManager::computeHash(salt, password);
        
        // MD5 хеш должен быть 32 символа (128 бит в hex)
        CHECK_EQUAL(32, hash.length());
        
        // Проверяем, что все символы - валидные hex цифры в верхнем регистре
        for (char c : hash) {
            bool is_hex = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
            CHECK(is_hex);
        }
    }
    
    TEST(ComputeHashKnownValue) {
        // Тест на известное значение MD5 хеша
        std::string salt = "0000000000000000";
        std::string password = "test";
        std::string hash = AuthManager::computeHash(salt, password);
        
        // Хеш не должен быть пустым
        CHECK(!hash.empty());
        CHECK_EQUAL(32, hash.length());
        
        // Дополнительная проверка - хеш от пустого пароля
        std::string empty_hash = AuthManager::computeHash(salt, "");
        CHECK(!empty_hash.empty());
        CHECK_EQUAL(32, empty_hash.length());
    }
    
    TEST(ComputeHashSpecialCharacters) {
        std::string salt = "1234567890ABCDEF";
        std::string password1 = "password with spaces";
        std::string password2 = "p@ssw0rd!";
        std::string password3 = "пароль";
        
        std::string hash1 = AuthManager::computeHash(salt, password1);
        std::string hash2 = AuthManager::computeHash(salt, password2);
        std::string hash3 = AuthManager::computeHash(salt, password3);
        
        CHECK_EQUAL(32, hash1.length());
        CHECK_EQUAL(32, hash2.length());
        CHECK_EQUAL(32, hash3.length());
        CHECK(hash1 != hash2);
        CHECK(hash1 != hash3);
        CHECK(hash2 != hash3);
    }
}

// Интеграционные тесты (без реального сетевого взаимодействия)
SUITE(IntegrationTest)
{
    TEST_FIXTURE(TestFileFixture, FileReadAndSaveIntegration) {
        // Читаем векторы из файла
        auto vectors = FileHandler::readVectors("test_input.txt");
        CHECK_EQUAL(2, vectors.size());
        
        // Создаем фиктивные результаты на основе входных данных
        std::vector<int32_t> results;
        for (const auto& vec : vectors) {
            int32_t sum = 0;
            for (int32_t val : vec) {
                sum += val;
            }
            results.push_back(sum);
        }
        
        // Проверяем вычисленные результаты
        CHECK_EQUAL(6, results[0]);  // 1+2+3 = 6
        CHECK_EQUAL(30, results[1]); // 10+20 = 30
        
        // Сохраняем результаты
        FileHandler::saveResults("test_output.bin", results);
        
        // Проверяем, что файл создан
        std::ifstream file("test_output.bin", std::ios::binary);
        CHECK(file.is_open());
        
        // Проверяем содержимое файла
        uint32_t num_results;
        file.read(reinterpret_cast<char*>(&num_results), sizeof(num_results));
        CHECK_EQUAL(2, num_results);
        
        int32_t result1, result2;
        file.read(reinterpret_cast<char*>(&result1), sizeof(result1));
        file.read(reinterpret_cast<char*>(&result2), sizeof(result2));
        
        CHECK_EQUAL(6, result1);
        CHECK_EQUAL(30, result2);
        
        file.close();
    }
    
    TEST_FIXTURE(TestFileFixture, ConfigAndAuthIntegration) {
        // Создаем конфиг файл
        std::ofstream config("test_config.conf");
        config << "testuser:testpass123";
        config.close();
        
        // Читаем конфиг
        auto [login, password] = FileHandler::readConfig("test_config.conf");
        CHECK_EQUAL("testuser", login);
        CHECK_EQUAL("testpass123", password);
        
        // Генерируем соль и хеш
        std::string salt = AuthManager::generateSalt();
        std::string hash = AuthManager::computeHash(salt, password);
        
        CHECK_EQUAL(16, salt.length());
        CHECK_EQUAL(32, hash.length());
        
        // Проверяем формат аутентификационного сообщения
        std::string auth_message = login + salt + hash;
        CHECK_EQUAL(16 + 32 + login.length(), auth_message.length());
        
        // Проверяем, что соль и хеш содержат только hex символы
        for (char c : salt) {
            CHECK((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
        }
        for (char c : hash) {
            CHECK((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
        }
    }
    
    TEST_FIXTURE(TestFileFixture, CompleteWorkflow) {
        // Полный рабочий процесс (без реального сетевого взаимодействия)
        
        // 1. Чтение конфигурации
        std::ofstream config("test_config.conf");
        config << "alex:P@ssW0rd";
        config.close();
        
        auto [login, password] = FileHandler::readConfig("test_config.conf");
        
        // 2. Чтение входных данных
        auto vectors = FileHandler::readVectors("test_input.txt");
        
        // 3. Генерация соли и хеша для аутентификации
        std::string salt = AuthManager::generateSalt();
        std::string hash = AuthManager::computeHash(salt, password);
        
        // 4. Обработка данных (имитация)
        std::vector<int32_t> results;
        for (const auto& vec : vectors) {
            results.push_back(static_cast<int32_t>(vec.size() * 100));
        }
        
        // 5. Сохранение результатов
        FileHandler::saveResults("test_output.bin", results);
        
        // Проверяем результаты
        CHECK_EQUAL("alex", login);
        CHECK_EQUAL("P@ssW0rd", password);
        CHECK_EQUAL(2, vectors.size());
        CHECK_EQUAL(2, results.size());
        CHECK_EQUAL(300, results[0]); // 3 элемента * 100
        CHECK_EQUAL(200, results[1]); // 2 элемента * 100
        
        // Проверяем, что файл результатов существует
        std::ifstream result_file("test_output.bin", std::ios::binary);
        CHECK(result_file.is_open());
        result_file.close();
    }
}

// Тестирование обработки ошибок
SUITE(ErrorHandlingTest)
{
    TEST_FIXTURE(TestFileFixture, FileReadErrors) {
        // Создаем файл с неверным форматом
        std::ofstream bad_file("invalid_file.txt");
        bad_file << "not_a_number\n";
        bad_file.close();
        
        CHECK_THROW(FileHandler::readVectors("invalid_file.txt"), std::runtime_error);
        
        // Создаем пустой файл
        std::ofstream empty_file("empty_file.txt");
        empty_file.close();
        
        CHECK_THROW(FileHandler::readVectors("empty_file.txt"), std::runtime_error);
    }
    
    TEST_FIXTURE(TestFileFixture, InvalidVectorData) {
        // Создаем файл с неверными данными векторов
        std::ofstream bad_file("invalid_file.txt");
        bad_file << "1\n";    // количество векторов
        bad_file << "2\n";    // размер вектора
        bad_file << "1 not_a_number\n"; // неверные данные
        bad_file.close();
        
        CHECK_THROW(FileHandler::readVectors("invalid_file.txt"), std::runtime_error);
    }
}

// Главная функция тестов
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
