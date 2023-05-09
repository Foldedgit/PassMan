#include "BGVrns.hpp"
#include "Utifac.hpp"
#include "Pass.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <cctype>


#include <memory>

auto c="./hwllp";


int main() {

    auto Warning = "\n\n*************************************  Warning *************************************  \n\n"
                " *** This app doesn't offer any secret phrase reset and key recovery options***\n\n"
                " *** This app uses robust encryption algorithem against quantum computers attacks ***\n\n"
                " *** making it utterly impossible to retrieve your data if you forget more \n\n "
                " \t than 10% of your secret phrase or lose your key. ***"
                " \n\n*************************************  Warning ************************************* \n\n" 
                "\t\t email: Admin@ai4risk.com\n";
    std::cerr << Warning << std::endl;
    std::string Secret_phrase = "";
    std::string hashs_secret;
    u_int16_t i=0;

    std::string newuser;
    std::cout << "Are you new? <yes / (no or Enter)>";
    std::getline(std::cin, newuser);
    

    if (newuser == "yes"){
        std::cout << "*************************************  Warning  *************************************"<< std::endl;
        std::cout << "New keys will be created for you. \nWith new keys you're not be able to access to your old data files\n"<< std::endl;

        
        while (Secret_phrase.size()<2)
        {
            std::cout << "\n\nSecret Phrase example: embrace life, dance with the world's endless waltz \nSet a Secret Phrase: ";
            std::getline(std::cin, Secret_phrase);
            sleep(i);
            i++;
        }
        const std::string &keyroot = "";
        const std::string &keyHash ="";
        
        BGVrns bgvrns0(keyroot, keyHash);
        PassCreate(bgvrns0.HASHpk, bgvrns0.HASHsk, Secret_phrase);

        PassCheck(Secret_phrase, "./");
        
        std::string service="";
        std::string username="";
        std::string password="";
        while(service.size()<2){
            std::cout << "\nPassMan> Service Name (Example: Google):";
            std::getline(std::cin, service);
            usleep(100);
        }
        while(username.size()<2){
            std::cout << service<<": Username: (Example: Alison):";
            std::getline(std::cin, username);
            usleep(100);
        }
        while(password.size()<2){
            std::cout << service<<": Password: (Example: lTGhq2*#Z5vwU5kc1W2mDU65mpKL$WrMi&#3QxDOt5O*SRm6s4):";
            std::getline(std::cin, password);
            usleep(100);
        }
        bgvrns->encrypt("{,}" + service + "{,}" + "{;} username:" + username + "\t password:"  + password + "{;}");
        Secret_phrase = "";
    }

        
    i=0;
    uint8_t j=0;
    std::string keyroot="";
    for(;;) 
    {
        while (Secret_phrase=="")
        {
            std::cout << "\n\nSecret Phrase example: embrace life, dance with the world's endless waltz \nYour Secret Phrase please:";
            std::getline(std::cin, Secret_phrase);
            sleep(i);
            i++;
        }
        i=0;
        while(keyroot==""){
            std::cout << "Please enter your Skey <folder or parent folder> path: ";
            std::getline(std::cin, keyroot);
            sleep(i);
            i++;
        }

        PassCheck(Secret_phrase, keyroot);
        if(bgvrns->HASHsk != "") break;
        sleep(j);
        j++;

    }

    for(;;){
        std::string command;
        std::cout << "\nAvailable Commands: insert new \t find \t list all \t exit \n\nPassMan>";
        std::getline(std::cin, command);

        if (command == "exit") break;
        if (command != "insert new" && command != "list all" && command != "find") {
            std::cout << "\nWrong command! Try Again \n"; 
            continue;}

        while(command == "insert new"){
            std::cout << "\n\tinsert new\tor\texit\n";
            std::string service="";
            std::string username="";
            std::string password="";
            std::cout << "\nNew> Service Name (Example: Google):";
            std::getline(std::cin, service);
            if(service=="exit") break;
            std::cout << service <<": Username: (Example: Alison):";
            std::getline(std::cin, username);
            if(username=="exit") break;
            std::cout << service<<": Password: (Example: lTGhq2*#Z5vwU5kc1W2mDU65mpKL$WrMi&#3QxDOt5O*SRm6s4):";
            std::getline(std::cin, password);
            if(password=="exit") break;

            auto data = bgvrns->decrypt();
            bgvrns->encrypt(data + "\n" + "{,}" + service + "{,}" + "{;} username:" + username + "\t password:"  + password + "{;}");
            sleep(1);
        }
        while(command =="find")
        {
            auto data = bgvrns->decrypt();
            std::string service="";
            std::cout << "\nPassMan> Service Name (Example: Google):";
            std::getline(std::cin, service);
            if(service=="exit") break;

            std::regex pattern1(R"(\{,\}(.*?)\{,\})"); // Pattern to match any string inside curly braces

            std::vector<std::string> services;
            // Use the regex iterator to find all matches in the input string
            for (std::sregex_iterator it(data.begin(), data.end(), pattern1), end; it != end; ++it) {
                // Access the captured group (group 1) and add it to the vector
                services.push_back((*it)[1]);
            }

            // Print the captured values                

            std::regex pattern2(R"(\{;\}(.*?)\{;\})"); // Pattern to match any string inside curly braces

            std::vector<std::string> authentications;
            // Use the regex iterator to find all matches in the input string
            for (std::sregex_iterator it(data.begin(), data.end(), pattern2), end; it != end; ++it) {
                // Access the captured group (group 1) and add it to the vector
                authentications.push_back((*it)[1]);
            }

            auto similarity = 0.1;
            std::string qservice;
            int index=0;
            int i=0;
            for (const auto& value : services) {
                if(stringSimilarity(service, value)>similarity){
                    qservice = value;
                    similarity = stringSimilarity(service, value);
                    index = i;
                }
                i++;                    
            }
            
            if(index>=0 && similarity>0.5){
                std::cout << "\n\n" << std::endl;
                std::cout << qservice << ":\t" <<authentications[index]<< std::endl;                    
                std::cout << "\n\n" << std::endl;
            }
            else std::cout << "\nNo result for "<< service << std::endl;
        }
        if(command =="list all")
        {
            auto data = bgvrns->decrypt();
            std::string service="";

            std::regex pattern1(R"(\{,\}(.*?)\{,\})"); // Pattern to match any string inside curly braces

            std::vector<std::string> services;
            // Use the regex iterator to find all matches in the input string
            for (std::sregex_iterator it(data.begin(), data.end(), pattern1), end; it != end; ++it) {
                // Access the captured group (group 1) and add it to the vector
                services.push_back((*it)[1]);
            }

            std::regex pattern2(R"(\{;\}(.*?)\{;\})"); // Pattern to match any string inside curly braces

            std::vector<std::string> authentications;
            // Use the regex iterator to find all matches in the input string
            for (std::sregex_iterator it(data.begin(), data.end(), pattern2), end; it != end; ++it) {
                // Access the captured group (group 1) and add it to the vector
                authentications.push_back((*it)[1]);
            }

            int i=0;
            for(const auto& value : services)
            {
                std::cerr<< "\n" <<value<<": \t"<<authentications[i]<<std::endl;
                i++;
            }
            
        }
        

               
        
    }        
               

       

}