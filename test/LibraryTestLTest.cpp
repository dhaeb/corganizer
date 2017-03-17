#include "LTest.h"

bool is_leap_year(unsigned int year){
        return (year % 4 == 0 and year % 100 != 0) or year % 400 == 0;
}

TestSuite suite = {
     ltest().addTest("test is leap year 2014 : ", [](){
         return !is_leap_year(2014);
     }),
     ltest().addTest("test is leap year 1900 : ", [](){
         return !is_leap_year(1900);
     }),
     ltest().addTest("test is leap year 2000 : ", [](){
         return is_leap_year(2000);
     })
};

int main() {
    ltest().run(suite);
    std::cout << "asdf";
    return 0;
}
