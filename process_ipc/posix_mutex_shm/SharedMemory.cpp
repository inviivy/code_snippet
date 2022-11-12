#include "SharedMemory.hpp"
#include "Lock.hpp"

#include <fcntl.h>
#include <sys/mman.h> // shm_unlink
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

SharedMemory::SharedMemory(std::string key, size_t truncate_size) {
  ::shm_unlink(key.data()); // 为了后续的shm_open成功
  int fd = ::shm_open(key.data(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0) [[unlikely]] {
    std::cout << "shm_open " << key << "shared memory error\n";
    std::abort();
  }
  if (::ftruncate(fd, truncate_size) < 0) {
    std::cout << "ftruncate " << fd << " error\n";
    std::abort();
  }

  mmap_ptr =
      ::mmap(nullptr, truncate_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mmap_ptr == MAP_FAILED) {
    std::cout << "mmap error\n";
    std::abort();
  }
  shmkey = std::move(key);
  mmap_size = truncate_size;
}

SharedMemory::~SharedMemory() {
  if (mmap_ptr != nullptr) {
    ::munmap(mmap_ptr, mmap_size);
  }
  shm_unlink(shmkey.data());
}

/* 始终从头写入 */
ssize_t SharedMemory::WriteMemory(const char *ptr, size_t len) {
  if (len > mmap_size) {
    return -1;
  }
  SynLock *base = reinterpret_cast<SynLock *>(mmap_ptr);
  uint64_t *size = reinterpret_cast<uint64_t *>(base + 1);
  char *begin = reinterpret_cast<char *>(size + 1);
  *size = len;
  memcpy(begin, ptr, len);
  return static_cast<ssize_t>(len);
}

std::string SharedMemory::ReadMemory() {
  SynLock *base = reinterpret_cast<SynLock *>(mmap_ptr);
  uint64_t *len_ptr = reinterpret_cast<uint64_t *>(base + 1);
  const char *begin = reinterpret_cast<char *>(len_ptr + 1);
  std::cout << "str len = " << *len_ptr << '\n';
  return std::string(begin, *len_ptr);
}

// 初始进程调用, 不能重复调用
void SharedMemory::init_lock() { ::new (mmap_ptr) SynLock; }

SynLock &SharedMemory::getLock() {
  return *reinterpret_cast<SynLock *>(mmap_ptr);
}