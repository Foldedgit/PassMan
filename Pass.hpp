#ifndef PASS_HPP
#define PASS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <regex>

#include "Utifac.hpp"
#include "BGVrns.hpp"


bool PassCreate(std::string HASHpk, std::string HASHsk, std::string Secret_phrase);
std::shared_ptr<BGVrns> PassCheck(std::string Secret_Phrase, std::string keyroot);
std::string Secret_to_Hashs(std::string Secret_Phrase);



#endif
