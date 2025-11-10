#pragma once
#include <string>
#include <boost/program_options.hpp>

struct ClientParams {
    std::string server_address;
    uint16_t port = 33333;
    std::string input_file;
    std::string output_file;
    std::string config_file = "~/.config/vclient.conf";
};

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
