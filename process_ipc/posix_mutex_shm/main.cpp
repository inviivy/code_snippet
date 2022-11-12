#include "SharedMemory.hpp"
#include "Lock.hpp"

#include <iostream>
#include <thread>

int main() {
  SharedMemory shm("POSIX_IPC_TEST_01", 4096);
  shm.init_lock();
  shm.getLock().Lock();
  std::string str("abcdef");
  shm.WriteMemory(str.data(), str.length());
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(30s);
  shm.getLock().Unlock();

  return 0;
}