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

class MyServer: public httplib::Server{
    private:
        httplib::Server serv;
        InMemoryStorage *db = new InMemoryStorage();
    public:
        MyServer(const char* host, int port){

            serv.Put(R"(/(\w+))", [&](const httplib::Request&req, httplib::Response &res){
                this->set_item_handler(req, res);
            });

            serv.Get(R"(/(\w+)/(\w+))", [&](const httplib::Request&req, httplib::Response &res){
                this->get_item_handler(req, res);
            });
            
            serv.Delete(R"(/(\w+))", [&](const httplib::Request&req, httplib::Response &res){
                this->delete_item_handler(req, res);
            });
            serv.listen(host, port);
    };
    ~MyServer(){
        delete db;
    }   
    void set_item_handler(const httplib::Request&req, httplib::Response &res);
    void get_item_handler(const httplib::Request&req, httplib::Response &res);
    void delete_item_handler(const httplib::Request&req, httplib::Response &res);

};


