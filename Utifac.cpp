#include "Utifac.hpp"


std::shared_ptr<BGVrns> bgvrns;


//remove header and generate a temporary file
std::pair<std::string, std::string> solve_header(std::string binary_file_path){
    std::string temporary_file_path = binary_file_path + "tmp";
        // Step 1: Open the binary file with the header in read mode
    std::ifstream input_file(binary_file_path, std::ios::binary);

    // Step 2: Read the header size from the binary file
    char header_size_str[10];
    input_file.read(header_size_str, 10);
    size_t header_size = std::stoull(std::string(header_size_str, 10));

    // Step 3: Read the header content from the binary file into a std::string variable
    std::vector<char> header_content(header_size - 10);
    input_file.read(header_content.data(), header_size - 10);
    std::string header(header_content.begin(), header_content.end());

    // Step 4: Read the remaining content of the binary file into a std::vector<char>
    std::vector<char> original_content((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    // Step 5: Close the binary file
    input_file.close();

    // Step 6: Create a temporary binary file in write mode
    std::ofstream temporary_file(temporary_file_path, std::ios::binary);

    // Step 7: Write the original content to the temporary binary file
    temporary_file.write(original_content.data(), original_content.size());

    // Step 8: Close the temporary binary file
    temporary_file.close();

    return std::make_pair(header, temporary_file_path);

}

//generate header {hash_skey:}{hash_pkey}{hashs_secret:}
std::string add_header(std::string binary_file_path){

    // Step 0: Read the header_tmp file
    std::ifstream input_fileh("./header", std::ios::binary);
    std::string header((std::istreambuf_iterator<char>(input_fileh)), std::istreambuf_iterator<char>());
    input_fileh.close();


    // Step 1: Read the entire content of the binary file into a std::vector<char>
    std::ifstream input_filec(binary_file_path, std::ios::binary);
    std::vector<char> original_content((std::istreambuf_iterator<char>(input_filec)), std::istreambuf_iterator<char>());

    // Step 2: Close the binary file
    input_filec.close();

    // Step 3: Reopen the binary file in write mode
    std::ofstream output_file(binary_file_path, std::ios::binary | std::ios::trunc);
    // Step 4: Write the header to the binary file
    output_file.write(header.data(), header.size());

    // Step 5: Write the content of the original binary file to the binary file
    output_file.write(original_content.data(), original_content.size());

    // Step 6: Close the binary file
    output_file.close();    
    return header;
}



std::vector<std::string> split_string(const std::string& input) {
    std::vector<std::string> tokens;
    std::string lowercase_input = input;
    std::transform(lowercase_input.begin(), lowercase_input.end(), lowercase_input.begin(), ::tolower);

    std::string token;
    std::istringstream iss(lowercase_input);
    while (iss >> token) {
        token.erase(std::remove_if(token.begin(), token.end(), [](char c) { return !std::isalpha(c); }), token.end());
        tokens.push_back(token);
    }

    return tokens;
}

std::string sha3string(const std::string& input) {
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha3_512();
    EVP_DigestInit_ex(mdctx, md, NULL);

    EVP_DigestUpdate(mdctx, input.data(), input.size());

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    EVP_DigestFinal_ex(mdctx, hash, &hashLen);
    EVP_MD_CTX_free(mdctx);

    std::stringstream hashString;
    hashString << std::hex << std::setfill('0');
    for (size_t i = 0; i < hashLen; ++i) {
        hashString << std::setw(2) << static_cast<int>(hash[i]);
    }

    return hashString.str();
}


std::string sha3file(const std::string &filePath) {
    std::ifstream fileStream(filePath, std::ios::binary);
    if (!fileStream) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha3_512();
    EVP_DigestInit_ex(mdctx, md, NULL);

    const size_t bufferSize = 4096;
    std::vector<unsigned char> buffer(bufferSize);
    while (fileStream.read(reinterpret_cast<char*>(buffer.data()), bufferSize)) {
        EVP_DigestUpdate(mdctx, buffer.data(), fileStream.gcount());
    }
    EVP_DigestUpdate(mdctx, buffer.data(), fileStream.gcount());

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    EVP_DigestFinal_ex(mdctx, hash, &hashLen);
    EVP_MD_CTX_free(mdctx);

    std::stringstream hashString;
    hashString << std::hex << std::setfill('0');
    for (size_t i = 0; i < hashLen; ++i) {
        hashString << std::setw(2) << static_cast<int>(hash[i]);
    }

    return hashString.str();
}

std::string findFilesWithHash(const std::string &rootPath, const std::string &targetHash) {
    std::filesystem::path fpath;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(rootPath)) {
        if (entry.is_regular_file()) {
            std::string fileHash = sha3file(entry.path().string());
            if (fileHash == targetHash) {
                fpath = entry.path();
                break;
            }
        }
    }

    return fpath.string();
}

std::string createFileName() {
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    std::ostringstream fileName;
    fileName << std::setfill('0')
             << std::setw(2) << localTime->tm_mday
             << std::put_time(localTime, "%B")
             << std::setw(2) << localTime->tm_hour
             << std::setw(2) << localTime->tm_min;

    return fileName.str();
}

int levenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t len1 = s1.size();
    size_t len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i) {
        dp[i][0] = i;
    }

    for (size_t j = 0; j <= len2; ++j) {
        dp[0][j] = j;
    }

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
        }
    }

    return dp[len1][len2];
}

double stringSimilarity(const std::string& s1, const std::string& s2) {
    int distance = levenshteinDistance(s1, s2);
    int maxLength = std::max(s1.length(), s2.length());
    return 1.0 - static_cast<double>(distance) / maxLength;
}

// Function to read the contents of a binary file
std::vector<unsigned char> readBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    file.close();
    return buffer;
}

std::vector<uint8_t> stringToBinary(const std::string& input) {
    std::vector<uint8_t> binaryData(input.begin(), input.end());
    return binaryData;
}