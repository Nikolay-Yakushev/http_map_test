#pragma once
#include<string>
#include "../libs/cpp-httplib/httplib.h"
#include<map>
#include "../headers/server.h"
#include<vector>
#define CPPHTTPLIB_OPENSSL_SUPPORT

class InMemoryStorage{
    private:
        std::map<std::string, std::map<std::string, std::string>> mem;
    public:
        InMemoryStorage(){}
    
    std::string get_value(std::string collection, std::string key);
    void set_value(std::string collection, std::string key, std::string value);
    void delete_collection(std::string collection);

};


    
