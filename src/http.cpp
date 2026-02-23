#include <curl/curl.h>
#include <string>
#include <iostream>
#include <algorithm>

// Callback function to capture response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// Callback function to capture headers
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, std::string* userp) {
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);
    
    // Check if it's Content-Type header
    if (header.find("Content-Type:") == 0) {
        // Extract the content type (skip "Content-Type: ")
        size_t pos = header.find(':');
        if (pos != std::string::npos) {
            *userp = header.substr(pos + 2);  // Skip ": "
            // Remove trailing \r\n
            userp->erase(std::remove(userp->begin(), userp->end(), '\r'), userp->end());
            userp->erase(std::remove(userp->begin(), userp->end(), '\n'), userp->end());
        }
    }
    
    return totalSize;
}

// Function to send HTTP request
std::string sendHttpRequest(const std::string& host, const std::string& port,
    long& statusCode, const std::string& path, std::string& contentType) {
    CURL* curl;
    CURLcode res;
    std::string response;
    statusCode = 0;  // Initialize
    contentType = "application/octet-stream";  // Default
    
    curl = curl_easy_init();
    if(curl) {
        std::string url = "http://" + host + ":" + port + path;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &contentType);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        
        res = curl_easy_perform(curl);
        
        if(res == CURLE_OK) {
            // Get the HTTP status code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
        }
        
        curl_easy_cleanup(curl);
    }
    return response;
}