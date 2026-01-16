#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// --- Helper: Trim whitespace ---
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// --- Helper: Load Webhook URL from Config ---
std::string loadWebhookUrl(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) throw std::runtime_error("Could not open config file: " + filepath);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                key = trim(key);
                if (key == "mattermost.webhook_url") {
                    return trim(value);
                }
            }
        }
    }
    throw std::runtime_error("Config missing 'mattermost.webhook_url'");
}

// --- Send Function ---
void sendWebhook(const std::string& url, const std::string& message, const std::string& channelOverride) {
    CURL* curl = curl_easy_init();
    if(curl) {
        json payload;
        payload["text"] = message;
        
        // Optional: Override the default channel if provided
        if (!channelOverride.empty()) {
            payload["channel"] = channelOverride;
        }

        std::string jsonStr = payload.dump();
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("CURL Connection Failed: " + std::string(curl_easy_strerror(res)));
        }
        if (http_code >= 400) {
            throw std::runtime_error("Mattermost Rejected Post (HTTP " + std::to_string(http_code) + ")");
        }
    }
}

// --- Main ---
int main(int argc, char* argv[]) {
    // Usage: ./SendMattermost <config_file> [optional_channel_name]
    if (argc < 2) {
        std::cerr << "Usage: SendMattermost <config_file> [channel_name]" << std::endl;
        return 1;
    }

    std::string confFile = argv[1];
    std::string channelTarget = (argc >= 3) ? argv[2] : "";

    try {
        // 1. Get URL
        std::string webhookUrl = loadWebhookUrl(confFile);

        // 2. Read Stdin
        std::string message;
        std::string line;
        while (std::getline(std::cin, line)) {
            if (!message.empty()) message += "\n";
            message += line;
        }

        if (message.empty()) return 0; // Nothing to send

        // 3. Send
        sendWebhook(webhookUrl, message, channelTarget);

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
