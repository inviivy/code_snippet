#include "address.pb.h"
#include <iostream>
#include <string>

int main() {
    tutorial::Person person;
    person.set_name("abc");
    person.set_id(1);
    person.set_email("abc@gmail.com");
    std::cout << person.DebugString() << '\n';
    std::cout << person.name() << '\n';
    std::string str;
    person.SerializeToString(&str);


    tutorial::Person person2;
    person2.ParseFromString(str);
    std::cout << person2.name() << '\n';
}