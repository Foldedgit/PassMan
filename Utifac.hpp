#ifndef UTIFAC_HPP
#define UTIFAC_HPP
#pragma once

#include "BGVrns.hpp"
//#include <string>
//#include <vector>
#include <algorithm>
//#include <cctype>
//#include <sstream>
#include <filesystem>
#include <iostream>
#include <openssl/evp.h>
#include <fstream>
//#include <memory>

extern std::shared_ptr<BGVrns> bgvrns;

std::pair<std::string, std::string> solve_header(std::string binary_file_path);
std::string add_header(std::string binary_file_path);

bool checkRowExists(std::vector<uint8_t> service_encrypted, std::vector<uint8_t> username_encrypted, 
                                std::vector<uint8_t> password_encrypted, const std::string& hashsk);
std::vector<uint8_t> stringToBinary(const std::string& input);                                
std::vector<std::string> split_string(const std::string& input);
std::string sha3file(const std::string &filePath);
std::string findFilesWithHash(const std::string &rootPath, const std::string &targetHash);
std::string createFileName();
std::string sha3string(const std::string& input);
int levenshteinDistance(const std::string& s1, const std::string& s2);
double stringSimilarity(const std::string& s1, const std::string& s2);
std::vector<unsigned char> readBinaryFile(const std::string& filename);

#endif 
