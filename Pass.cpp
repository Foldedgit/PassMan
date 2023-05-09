#include "Pass.hpp"



std::string Secret_to_Hashs(std::string Secret_Phrase){    
    std::string hashs_secret = sha3string(Secret_Phrase);
    std::vector<std::string> splited_secret = split_string(Secret_Phrase);
    for(std::string secret : splited_secret){
        hashs_secret +=  "," + sha3string(secret);
        for(char c : secret){
            std::ostringstream oss;
            oss << c;
            std::string cstring = oss.str();
            hashs_secret += ","+ sha3string(cstring);
        }
    }
    return hashs_secret;
}

bool PassCreate(std::string HASHpk, std::string HASHsk, std::string Secret_phrase){
    auto hashs_secret = Secret_to_Hashs(Secret_phrase);
    auto header = ("{" + HASHsk + "}" +
                         "{" + HASHpk +"}" + 
                         "{" + hashs_secret +"}");

    // Calculate the size of the final header (including the size field)
    size_t header_size_field_length = 10;
    size_t header_size = header.size() + header_size_field_length;

    // Convert the header size to a fixed-width string representation (e.g., 10 characters)
    std::stringstream ss;
    ss << std::setw(header_size_field_length) << std::setfill('0') << header_size;
    std::string header_size_str = ss.str();

    // Prepend the header size string to the header
    header = header_size_str + header;
    std::ofstream output_file("./header", std::ios::binary | std::ios::trunc);
    output_file.write(header.data(), header.size());
    // Step 6: Close the binary file
    output_file.close();

    return true;
}

std::shared_ptr<BGVrns> PassCheck(std::string Secret_Phrase, std::string keyroot){
    // Step 0: Read the header_tmp file
    std::ifstream input_fileh("./header", std::ios::binary);
    std::string header((std::istreambuf_iterator<char>(input_fileh)), std::istreambuf_iterator<char>());
    input_fileh.close();


    std::regex pattern(R"(\{([^}]+)\})"); // Pattern to match any string inside curly braces
    std::smatch match;

    std::vector<std::string> values;
    // Iterate through all matches in the input string
    while (std::regex_search(header, match, pattern)) {
        values.push_back(match[1].str());
        header = match.suffix().str();
    }


    std::string hashsk = values[0];
    std::string hashpk = values[1];
    std::string hashs_secret = values[2];

    std::string Input_secret = Secret_to_Hashs(Secret_Phrase);
    
    if(stringSimilarity(hashs_secret, Input_secret)<0.9){
        std::cerr << "Wrong Secret Phrase. Try again!" << std::endl;
        return bgvrns;

    }     


    try{
        bgvrns = std::make_shared<BGVrns>(keyroot, hashsk);
        bgvrns->HASHpk = hashpk;
        bgvrns->HASHsk = hashsk;

            
        } catch (const std::runtime_error &e) {
            std::cout << "Exception caught: " << e.what() << std::endl;
        }   
    
    return bgvrns;

}

