#pragma once


#include "openfhe.h"
//#include "ciphertext-ser.h"
//#include "cryptocontext-ser.h"
//#include "key/key-ser.h"
//#include "scheme/bfvrns/bfvrns-ser.h"
#include "scheme/bgvrns/bgvrns-ser.h"
//#include <filesystem>
//#include <iostream>
//#include <vector>
//#include <openssl/evp.h>
//#include "ciphertext.h"
//#include <string>
//#include <fstream>
//#include <memory>


namespace fs = std::filesystem;
using namespace lbcrypto;
class BGVrns{
    
    public:
        BGVrns(const std::string &keyroot, const std::string &keyHash);
        ~BGVrns();
        KeyPair<DCRTPoly> keyPair;
        std::string encrypt(std::string text);
        std::string decrypt(); 
        std::string HASHpk;
        std::string HASHsk;
        
        
        

    private:

    CCParams<CryptoContextBGVRNS> parameters;
    CryptoContext<DCRTPoly> cryptoContext;
    PrivateKey<DCRTPoly> sk;
    PublicKey<DCRTPoly> pk;
    std::string Skey_path;
    std::string Pkey_path;
    std::string keyroot_;
    
    
    void key_gen();


};