#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// Use the nlohmann JSON namespace for convenience
using json = nlohmann::json;

// --- Helper: Struct to hold config data ---
struct AppConfig {
    std::string server;
    std::string username;
    std::string password;
};

// --- Helper: CURL Write Callback (to read response body) ---
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// --- Helper: CURL Header Callback (to extract Token from headers) ---
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, std::string* token) {
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);
    
    // Mattermost returns the session token in the "Token" header
    std::string prefix = "Token: ";
    if (header.rfind(prefix, 0) == 0) { // Starts with "Token: "
        *token = header.substr(prefix.length());
        // Remove trailing newline/carriage return
        while (!token->empty() && (token->back() == '\n' || token->back() == '\r')) {
            token->pop_back();
        }
    }
    return totalSize;
}

// --- Helper: Trim whitespace from config strings ---
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// --- Function: Load Config ---
AppConfig loadConfig(const std::string& filepath) {
    AppConfig config;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filepath);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                key = trim(key);
                value = trim(value);
                if (key == "mattermost.server") config.server = value;
                else if (key == "mattermost.username") config.username = value;
                else if (key == "mattermost.password") config.password = value;
            }
        }
    }
    return config;
}

// --- Function: Login to Mattermost and get Token ---
std::string login(const AppConfig& config) {
    CURL* curl = curl_easy_init();
    std::string token;
    std::string responseBody;

    if(curl) {
        std::string url = config.server + "/api/v4/users/login";
        
        // Create JSON payload
        json payload;
        payload["login_id"] = config.username;
        payload["password"] = config.password;
        std::string jsonStr = payload.dump();

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Capture Header (to find Token) and Body (for errors)
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &token);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("CURL failed: " + std::string(curl_easy_strerror(res)));
        }
        if (http_code != 200) {
            throw std::runtime_error("Login failed (HTTP " + std::to_string(http_code) + ")");
        }
        if (token.empty()) {
            throw std::runtime_error("Login successful but no Token header received.");
        }
    }
    return token;
}

// --- Function: Send Message ---
void sendMessage(const AppConfig& config, const std::string& token, const std::string& channelId, const std::string& message) {
    CURL* curl = curl_easy_init();
    if(curl) {
        std::string url = config.server + "/api/v4/posts";

        // Create JSON payload
        json payload;
        payload["channel_id"] = channelId;
        payload["message"] = message;
        std::string jsonStr = payload.dump();

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string authHeader = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, authHeader.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        
        // Optional: Check success
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Message send failed: " + std::string(curl_easy_strerror(res)));
        }
        if (http_code != 201) { // 201 Created is the success code for posts
            throw std::runtime_error("Mattermost rejected post (HTTP " + std::to_string(http_code) + ")");
        }
    }
}

// --- Main ---
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: SendMattermost <config_file> <channel_id>" << std::endl;
        return 1;
    }

    std::string confFile = argv[1];
    std::string channelId = argv[2];

    try {
        // 1. Load Config
        AppConfig config = loadConfig(confFile);

        // 2. Read stdin
        std::string message;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!message.empty()) {
                message += "\n";
            }
            message += line;
        }

        if (message.empty()) {
            // No input, nothing to send
            return 0; 
        }

        // 3. Login
        std::string token = login(config);

        // 4. Send
        sendMessage(config, token, channelId, message);

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
