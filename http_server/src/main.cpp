#include "../headers/server.h"
void InMemoryStorage::set_value(std::string collection, std::string key, std::string value){
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

std::string InMemoryStorage::get_value(std::string collection, std::string key){
    if (mem.find(collection)!=mem.end()){ 
        std::cout<<"first"<<std::endl;
        if(mem[collection].find(key)!=mem[collection].end()){
            std::cout<<"sec"<<std::endl;
            return mem[collection][key];
        }
    }
    return "None";
}

void InMemoryStorage::delete_collection(std::string collection){
    std::cout<<"collection is= "<<collection<<std::endl;
    if (mem.find(collection)!=mem.end()){
        mem.erase(collection); 
    }
}

int main(){
    httplib::Server svr; 
    InMemoryStorage* redis_memory =  new InMemoryStorage();

    svr.Get("/hi", [](const httplib::Request& req, httplib::Response& res) {
    std::cout<<"hee"<<std::endl;
    res.set_content("Hello World!", "text/plain");
    });

     svr.Put(R"(/(\w+))", [&](
        const httplib::Request&req, httplib::Response &res
                        ) 
                            {   
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
    
                                    redis_memory->set_value(collection, key, data);
                                    res.status=200;
                                }else{
                                    res.set_content("No data was provided", "text/plain");
                                    res.status=400;
                                }
                            }
            );
    
    svr.Get(R"(/(\w+)/(\w+))", [&](
        const httplib::Request&req, httplib::Response &res
                        )   {    
                                auto collection = req.matches[1];
                                auto key = req.matches[2];
                                std::cout<<collection<<::std::endl;
                                std::cout<<key<<::std::endl;
                                std::string value = redis_memory->get_value(collection, key);
                                if (value=="None"){
                                    res.body=value;
                                    res.set_content(res.body, "text/plain");
                                    res.status=404;
                                }else{
                                    res.body=value;
                                    res.set_content(res.body, "text/plain");
                                    res.status=200;
                                }
                            }
            );
    svr.Delete(R"(/(\w+))", [&](
        const httplib::Request&req, httplib::Response &res
                        )   {   
                                auto collection = req.matches[1];
                                std::cout<<collection<<std::endl;
                                redis_memory->delete_collection(collection);
                                res.set_content("ok", "text/plain");
                                res.status=200;

                            }
            );
    svr.listen("127.0.0.1", 8080);
    delete redis_memory;
}