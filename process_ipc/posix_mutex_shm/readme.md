# 共享内存demo

+ 使用共享内存创建一个用于跨线程通信的媒介
  + 在共享内存中为mutex和condition_variable预留空间
  + 在初始化posix的mutex和condition_variable时设置PTHREAD_PROCESS_SHARED属性
  + 执行正常的lock/unlock