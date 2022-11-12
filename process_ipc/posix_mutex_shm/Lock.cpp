#include "Lock.hpp"

#include <pthread.h>

#include <cstdlib>
#include <iostream>

SynLock::SynLock() {
  pthread_condattr_t condattr; // 定义条件变量的属性对象
  pthread_condattr_init(&condattr);
  

  pthread_mutexattr_t mutexattr; // 定义互斥变量的属性对象
  pthread_mutexattr_init(&mutexattr);
  //判断系统是否支持用于进程间同步，并且设置它们的属性为进程间使用
  //#ifdef _POSIX_THREAD_PROCESS_SHARED
  pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);
  pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
  // #else
  //   std::cout << "don't support process shared\n";
  //   std::abort();
  // #endif

  // 根据属性来初始化条件变量和互斥变量
  pthread_cond_init(&m_cond, &condattr);
  pthread_mutex_init(&m_mutex, &mutexattr);
}

SynLock::~SynLock() {
  pthread_cond_destroy(&m_cond);
  pthread_mutex_destroy(&m_mutex);
}

void SynLock::Lock() { pthread_mutex_lock(&m_mutex); }

void SynLock::Unlock() { pthread_mutex_unlock(&m_mutex); }

void SynLock::Wait() { pthread_cond_wait(&m_cond, &m_mutex); }

void SynLock::NotifyOne() { pthread_cond_signal(&m_cond); }

void SynLock::NotifyAll() { pthread_cond_broadcast(&m_cond); }