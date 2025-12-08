CXX = g++
CXXFLAGS = -O2 -Wall -std=c++17 -DCRYPTOPP_ENABLE_NAMESPACE_WEAK=1
LDFLAGS = -lcrypto++ -lboost_program_options
TEST_LDFLAGS = -lcrypto++ -lboost_program_options -lUnitTest++

SOURCES = main.cpp file_handler.cpp connection_manager.cpp user_interface.cpp auth_manager.cpp
HEADERS = file_handler.h connection_manager.h user_interface.h auth_manager.h
TARGET = client

TEST_SOURCES = test_main.cpp file_handler.cpp user_interface.cpp auth_manager.cpp connection_manager.cpp
TEST_TARGET = test_client

# Цель по умолчанию - просто собираем клиента
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

$(TEST_TARGET): $(TEST_SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(TEST_LDFLAGS)

# Тестирование
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Создание демо файлов (опционально)
demo:
	@echo "user:P@ssW0rd" > ~/.config/vclient.conf
	@echo "✓ Создан конфиг файл: ~/.config/vclient.conf"

# Очистка
clean:
	rm -f $(TARGET) $(TEST_TARGET) *.bin *.txt test_*.txt test_*.bin

# Пересборка
rebuild: clean all

.PHONY: all clean test demo rebuild
