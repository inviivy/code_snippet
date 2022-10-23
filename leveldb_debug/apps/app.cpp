#include <assert.h>
#include <iostream>
#include <leveldb/db.h>
#include <string.h>

// using namespace leveldb;

int main() {
  leveldb::DB *db = nullptr;
  leveldb::Options options;
  // 打开一个数据库, 不存在就创建
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
  assert(status.ok());

  // 插入一个键值对
  status = db->Put(leveldb::WriteOptions(), "hello", "levelDB");
  assert(status.ok());

  // 读取键值对
  std::string value;
  status = db->Get(leveldb::ReadOptions(), "hello", &value);

  assert(status.ok());
  std::cout << value << '\n';

  delete db;
  return 0;
}