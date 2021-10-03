#pragma once
#include<string>
#include "../libs/cpp-httplib/httplib.h"
#include<map>
#include "../libs/argparse/include/argparse/argparse.hpp"
#include <stdlib.h>
#include <stdio.h>
#include<vector>

#include <sw/redis++/redis++.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT

class IMyStorage
{   
    public:
        virtual ~IMyStorage(){};

    virtual std::optional<std::string> get_value(std::string collection, std::string key)=0;
    virtual void set_value(std::string collection, std::string key, std::string value)=0;
    virtual void  delete_collection(std::string collection)=0;
};


class InMemoryStorage: public IMyStorage
{
    private:
        std::map<std::string, std::map<std::string, std::string>> mem;
    public:
        InMemoryStorage(){};
        ~InMemoryStorage(){};
    
    virtual std::optional<std::string> get_value(std::string collection, std::string key) override;
    void set_value(std::string collection, std::string key, std::string value) override;
    void delete_collection(std::string collection) override;

};

class RedisStorage: public IMyStorage
{   
    private:
        const char* redis_addr = "tcp://127.0.0.1:6379";
        sw::redis::Redis red =  sw::redis::Redis(redis_addr);
    public:
        RedisStorage(){};

        RedisStorage(std::string tcp_string)
        {
            auto red = sw::redis::Redis(tcp_string);
            
        }   
        ~RedisStorage(){

        }
    
    virtual std::optional<std::string> get_value(std::string collection, std::string key) override;
    void set_value(std::string collection, std::string key, std::string value) override;
    void delete_collection(std::string collection) override;

};

class MyServer: public httplib::Server
{
    private:
        httplib::Server serv;
        IMyStorage &db;
    public:
        MyServer(const char* host, int port, IMyStorage& storage)
        :db(storage)
        {   
            serv.Put(R"(/(\w+))", [&](const httplib::Request&req, httplib::Response &res){
                set_item_handler(req, res);
            });

            serv.Get(R"(/(\w+)/(\w+))", [&](const httplib::Request&req, httplib::Response &res){
                get_item_handler(req, res);
            });
            
            serv.Delete(R"(/(\w+))", [&](const httplib::Request&req, httplib::Response &res){
                delete_item_handler(req, res);
            });
            serv.listen(host, port);
        };   
    void set_item_handler(const httplib::Request&req, httplib::Response &res);
    void get_item_handler(const httplib::Request&req, httplib::Response &res);
    void delete_item_handler(const httplib::Request&req, httplib::Response &res);

};


