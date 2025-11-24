CXX = g++
CXXFLAGS = -O2 -Wall -std=c++17 -DCRYPTOPP_ENABLE_NAMESPACE_WEAK=1
LDFLAGS = -lcrypto++ -lboost_program_options
TEST_LDFLAGS = -lcrypto++ -lboost_program_options -lUnitTest++

SOURCES = main.cpp file_handler.cpp connection_manager.cpp user_interface.cpp auth_manager.cpp
HEADERS = file_handler.h connection_manager.h user_interface.h auth_manager.h
TARGET = client

TEST_SOURCES = test_main.cpp file_handler.cpp user_interface.cpp auth_manager.cpp
TEST_TARGET = test_client

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

$(TEST_TARGET): $(TEST_SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(TEST_LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) test_vectors.txt results.bin *.bin *.txt test_config.conf

install-config:
	mkdir -p ~/.config
	echo "user:P@ssW0rd" > ~/.config/vclient.conf
	echo "Конфигурационный файл создан: ~/.config/vclient.conf"

test-data:
	echo "3" > test_vectors.txt
	echo "4" >> test_vectors.txt
	echo "1 2 3 4" >> test_vectors.txt
	echo "3" >> test_vectors.txt
	echo "10 20 30" >> test_vectors.txt
	echo "2" >> test_vectors.txt
	echo "100 200" >> test_vectors.txt
	echo "Тестовые данные созданы: test_vectors.txt"

.PHONY: clean install-config test-data test
