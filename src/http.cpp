#include <curl/curl.h>
#include <string>
#include <iostream>

// Callback function to capture response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// Function to send HTTP request
std::string sendHttpRequest(const std::string& host, const std::string& port,
    long& statusCode, const std::string& path) {
    CURL* curl;
    CURLcode res;
    std::string response;
    statusCode = 0;  // Initialize
    
    curl = curl_easy_init();
    if(curl) {
        std::string url = "http://" + host + ":" + port + path;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
        
        res = curl_easy_perform(curl);
        
        if(res == CURLE_OK) {
            // Get the HTTP status code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
        }
        
        curl_easy_cleanup(curl);
    }
    return response;  // Body only, not status code
}