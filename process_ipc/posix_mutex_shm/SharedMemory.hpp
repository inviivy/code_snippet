//#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>

#include <string>

class SynLock;

struct SharedMemory {
  // 默认将共享内存的大小设置为4096字节
  SharedMemory(std::string key, size_t truncate_size = 4096);
  ~SharedMemory();

  ssize_t WriteMemory(const char *ptr, size_t len);
  std::string ReadMemory();

  void init_lock();

  SynLock &getLock();

private:
  std::string shmkey;
  void *mmap_ptr = nullptr;
  size_t mmap_size;
};