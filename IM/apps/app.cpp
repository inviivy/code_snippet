#include "address.pb.h"
#include <iostream>
#include <string>

// mysql
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

void access_protobuf() {
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
  if (person2.has_addr()) {
    std::cout << "has addr\n";
  } else {
    std::cout << "has no addr\n";
  }
}

void access_mysql() {
  try {
    // 改成你的信息
    Session sess(SessionOption::USER, "root", SessionOption::PWD, "123456",
                 SessionOption::HOST, "172.30.16.1", SessionOption::PORT, 33060,
                 SessionOption::DB, "demo");

    auto result = sess.sql("select * from t").execute();

    for (auto row : result.fetchAll()) {
      std::cout << row[0] << " " << row[1] << " " << row[2] << "\n";
    }

  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}

int main() {
  access_protobuf();
  access_mysql();
}