#include "../headers/server.hpp"
#include <optional>


void* InMemoryStorage::set_value(std::string collection, std::string key, std::string value){
        // key=abc value 
        if (mem.find(collection)!=mem.end()){
            mem[collection][key]=value;
            std::cout<<"set to existed"<<std::endl;
        }else{
             // {abc: {a: 1}}
            std::cout<<"set to new"<<std::endl;
            std::map<std::string, std::string> in_mem;
            in_mem[key] = value;
            mem[collection] = in_mem;
        }
}

std::string* InMemoryStorage::get_value(std::string collection, std::string key){
    std::string *res = nullptr;
    if (mem.find(collection)!=mem.end()){ 
        std::cout<<"first"<<std::endl;
        if(mem[collection].find(key)!=mem[collection].end()){
            std::cout<<"sec"<<std::endl;
            res = &mem[collection][key];
            return res;
        }
    }
    return res;
}


void* InMemoryStorage::delete_collection(std::string collection){
    std::cout<<"collection is= "<<collection<<std::endl;
    if (mem.find(collection)!=mem.end()){
        mem.erase(collection); 
    }
}



std::string* RedisStorage::get_value(std::string collection, std::string key){
    std::string *res = nullptr; 
    auto val = red.hget(collection, key);
    if(val){
       res = &(*val);
       std::cout<<"res= "<<*res<<std::endl;
    }
    return res;

}

void* RedisStorage::set_value(std::string collection, std::string key, std::string value){
    red.hset(collection, key, value);
}


void* RedisStorage::delete_collection(std::string collection){
    red.del(collection);
}

void MyServer::set_item_handler(const httplib::Request&req, httplib::Response &res){  
    auto collection = req.matches[1];
    std::cout<<collection<<std::endl;
    std::string data = req.body;
    
    //find key e.g "a" and value
    std::string delim = "=";
    std::size_t pos = 0;
    if((pos = data.find(delim)) != std::string::npos){
        std::size_t delim_pos = data.find(delim);
        std::string key = data.substr(0, delim_pos);
        data.erase(0, delim_pos+delim.length());
        db.set_value(collection, key, data);
        res.status=200;
    }else{
        res.set_content("No data was provided", "text/plain");
        res.status=400;
    }

}

void MyServer::get_item_handler(const httplib::Request&req, httplib::Response &res){  
   auto collection = req.matches[1];
    auto key = req.matches[2];
    std::cout<<collection<<::std::endl;
    std::cout<<key<<::std::endl;
    std::string *value = db.get_value(collection, key);
    if (value==nullptr){
        res.body="None";
        res.set_content(res.body, "text/plain");
        res.status=404;
    }else{
        try{
            std::string response = *value;
            std::cout<<"response="<<response<<std::endl;
            res.body=response;
        }catch(const std::exception &err){
            std::cout<<err.what()<<std::endl;
        }

        res.set_content(res.body, "text/plain");
        res.status=200;
    }

}

void MyServer::delete_item_handler(const httplib::Request&req, httplib::Response &res){
    auto collection = req.matches[1];
    std::cout<<collection<<std::endl;
    db.delete_collection(collection);
    res.set_content("ok", "text/plain");
    res.status=200;
}


int main(){
    IMyStorage *storage = new RedisStorage();
    //MyStorage *storage  = new InMemoryStorage();

    MyServer *serv = new MyServer("127.0.0.1", 8080, *storage);
    delete serv;
}