#pragma once

#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct SynLock {
  SynLock();
  ~SynLock();

  void Lock();
  void Unlock();
  void Wait();
  void NotifyOne();
  void NotifyAll();

private:
  ::pthread_cond_t m_cond;
  ::pthread_mutex_t m_mutex;
};