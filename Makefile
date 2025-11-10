CXX = g++
CXXFLAGS = -O2 -Wall -std=c++17 -DCRYPTOPP_ENABLE_NAMESPACE_WEAK=1
LDFLAGS = -lcrypto++ -lboost_program_options

SOURCES = main.cpp file_handler.cpp connection_manager.cpp user_interface.cpp auth_manager.cpp
HEADERS = file_handler.h connection_manager.h user_interface.h auth_manager.h
TARGET = client

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET) test_vectors.txt test_results.bin

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

.PHONY: clean install-config test-data
