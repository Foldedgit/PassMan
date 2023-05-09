#include "BGVrns.hpp"
#include "openfhe.h"
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/bfvrns/bfvrns-ser.h"
#include "scheme/bgvrns/bgvrns-ser.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <openssl/evp.h>
#include "Utifac.hpp"
#include "ciphertext.h"
#include <string>
#include <fstream>

BGVrns::BGVrns(const std::string &keyroot, const std::string &keyHash){
    parameters.SetMultiplicativeDepth(20);
    parameters.SetPlaintextModulus(256);

    cryptoContext = GenCryptoContext(parameters);
    // Enable features that you wish to use
    cryptoContext->Enable(PKE);
    cryptoContext->Enable(KEYSWITCH);
    cryptoContext->Enable(LEVELEDSHE);

    

    if(keyHash=="" && keyroot==""){
        key_gen();
        std::cerr << "new key created" << std::endl;
        keyroot_ = "./";
    }else{ keyroot_ = keyroot;}     
    }

BGVrns::~BGVrns(){}




void BGVrns::key_gen(){
    std::cerr << "creating new key..." << std::endl;
    std::string filename = createFileName();
    std::cerr << "creating file name..."<< filename << std::endl;
    keyPair = cryptoContext->KeyGen();

    if (!Serial::SerializeToFile("./" + filename , keyPair.publicKey, SerType::BINARY)) {
        std::cerr << "Error writing serialization of public key to key-public.txt" << std::endl;
    }
    std::cout << "The public key has been serialized." << std::endl;
    HASHpk = sha3file("./" + filename);
    

    // Serialize the secret key    
    if (!Serial::SerializeToFile("./" + filename + "Skey", keyPair.secretKey, SerType::BINARY)) {
        std::cerr << "Error writing serialization of private key to key-private.txt" << std::endl;
    }
    HASHsk = sha3file("./" + filename + "Skey");
}



std::string BGVrns::encrypt(std::string text){
    Pkey_path = findFilesWithHash(keyroot_, HASHpk);
    if(Pkey_path == "") throw std::runtime_error("Error in constructor");

    if (Serial::DeserializeFromFile(Pkey_path, pk, SerType::BINARY) == false) {
        std::cerr << "Could not read public key" << std::endl;
    }

    Plaintext plaintext               = cryptoContext->MakeStringPlaintext(text);
    // The encoded vectors are encrypted
    auto ciphertext = cryptoContext->Encrypt(pk, plaintext);
    if (!Serial::SerializeToFile("./data", ciphertext, SerType::BINARY)) {
        std::cerr << "Error writing serialization of encrypted data to data" << std::endl;
    }


    if (!Serial::SerializeToFile("./Data/" + createFileName(), ciphertext, SerType::BINARY)) {
        std::cerr << "Error writing serialization of ./Data/..." << std::endl;
    }

    

    //Add header to file {hash_skey:}{hash_pkey}{hashs_secret:}
    auto header = add_header("./data");
    header = add_header("./Data/" + createFileName());

    std::string s = Serial::SerializeToString(ciphertext);
    
    return s;
}


std::string BGVrns::decrypt(){
    std::pair<std::string, std::string> content_pair = solve_header("./data");
    lbcrypto::Ciphertext<lbcrypto::DCRTPoly> cipher;
    
    Skey_path = findFilesWithHash(keyroot_, HASHsk);
    if(Skey_path == "") throw std::runtime_error("Error in constructor");
    if (Serial::DeserializeFromFile(Skey_path, sk, SerType::BINARY) == false) {
        std::cerr << "Could not read secret key" << std::endl;
    }


    if (Serial::DeserializeFromFile(content_pair.second, cipher, SerType::BINARY) == false) {
        std::cerr << "Could not read ciphertext" << std::endl;
    }    
    
    //lbcrypto::Serial::DeserializeFromString(cipher, ciphertext);
    Plaintext plaintext;
    cryptoContext->Decrypt(sk, std::dynamic_pointer_cast<const lbcrypto::CiphertextImpl<lbcrypto::DCRTPoly>>(cipher) , &plaintext);
    std::string strintext = plaintext->GetStringValue();


    return strintext;
}






