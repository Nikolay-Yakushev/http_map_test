#include "../libs/googletest/googletest/include/gtest/gtest.h"
#include "../headers/server.h"

class TestCaseServer: public ::testing::Test
{
    protected:
        void SetUp(){
            
        }
        InMemoryStorage* redis_memory =  new InMemoryStorage();

        void TearDown(){
         
        }

};

int main(){
    ::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}