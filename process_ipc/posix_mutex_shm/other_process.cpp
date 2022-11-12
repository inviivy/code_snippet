#include "Lock.hpp"
#include "SharedMemory.hpp"

#include <iostream>
#include <thread>

int main() {
  SharedMemory shm("POSIX_IPC_TEST_01", 4096);
  while (true) {
    shm.getLock().Lock();
    std::string readStr = shm.ReadMemory();
    shm.getLock().Unlock();
  }

  std::cout << ".................\n";

  return 0;
}