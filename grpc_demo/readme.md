# grpc install

```
export MY_INSTALL_DIR=$HOME/grpc_install_directory
mkdir -p $MY_INSTALL_DIR
export PATH="$MY_INSTALL_DIR/bin:$PATH"

# 依赖cmake
sudo apt install -y cmake
sudo apt install -y build-essential autoconf libtool pkg-config

git clone --recurse-submodules -b v1.53.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc

cd grpc
mkdir -p cmake/build
pushd cmake/buildcmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      ../..
make
make install
```

# reference
+ [quickstart](https://grpc.io/docs/languages/cpp/quickstart/)
