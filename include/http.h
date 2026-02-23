#ifndef HTTP_H
#define HTTP_H
#include <iostream>
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, std::string* userp);
std::string sendHttpRequest(const std::string& host, const std::string& port, long& statusCode, const std::string& path, std::string& contentType);
#endif