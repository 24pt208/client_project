#include "auth_manager.h"
#include <iomanip>
#include <sstream>
#include <random>

#include <cryptopp/hex.h>
#include <cryptopp/md5.h>
#include <cryptopp/osrng.h>

namespace CPP = CryptoPP;

std::string AuthManager::generateSalt() {
    CPP::AutoSeededRandomPool prng;
    uint64_t salt_value;
    prng.GenerateBlock(reinterpret_cast<CPP::byte*>(&salt_value), sizeof(salt_value));
    
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << salt_value;
    return oss.str();
}

std::string AuthManager::computeHash(const std::string& salt, const std::string& password) {
    CPP::Weak::MD5 hash;
    
    std::string data = salt + password;
    CPP::byte digest[CPP::Weak::MD5::DIGESTSIZE];
    
    hash.CalculateDigest(digest, 
                        reinterpret_cast<const CPP::byte*>(data.data()), 
                        data.size());
    
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');
    for (int i = 0; i < CPP::Weak::MD5::DIGESTSIZE; ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(digest[i]);
    }
    
    return oss.str();
}
