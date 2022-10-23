include(FetchContent)

FetchContent_Declare(
    leveldb
  GIT_REPOSITORY https://github.com/google/leveldb.git
  GIT_TAG 1.23)
# https://gitlab.kitware.com/cmake/cmake/-/issues/20579
# 
FetchContent_MakeAvailable(leveldb)
