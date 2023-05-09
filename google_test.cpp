#include <gtest/gtest.h>
#include "BGVrns.hpp"
#include "Pass.hpp"


class BGVrnsTest : public ::testing::Test{
    protected:
    BGVrns* bgvrns;
    std::string text1 = "Hello Word";
    std::string text2 = "Helloo World";
    std::string cipher1, cipher2, strtext2;
        void SetUp() override{
            bgvrns = new BGVrns("", "");
            auto Secret_phrase = "Dance with";
            PassCreate(bgvrns->HASHpk, bgvrns->HASHsk, Secret_phrase);
            cipher1 = bgvrns->encrypt(text1);
            cipher2 = bgvrns->encrypt(text2);
            strtext2 = bgvrns->decrypt();
        }

        void TearDown() override{

        }

};

//test BGVrns
TEST_F(BGVrnsTest, uniquness) {    

    EXPECT_STRNE(cipher1.c_str(), cipher2.c_str());
}

//test BGVrns
TEST_F(BGVrnsTest, encryption) {

    EXPECT_STRNE(text1.c_str(), cipher1.c_str());
}

//test BGVrns
TEST_F(BGVrnsTest, decryption) {
 
    EXPECT_STREQ(strtext2.c_str(), text2.c_str());
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
