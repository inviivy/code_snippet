# 安装

```
// download curl-7.87.0.tar.gz
// tar zxvf curl-7.87.0.tar

cd curl-7.87.0
sudo apt install openssl
# enable ssl and static
# ./configure --without-zlib --with-openssl --disable-shared
./configure --with-openssl --enable-static
make
sudo make install
```

# 静态编译openssl & libcurl & demo

## 编译openssl静态库
```
# 下载 openssl
git clone -b openssl-3.1 --depth=1 https://github.com/openssl/openssl.git


git clone https://github.com/curl/curl.git --depth=1
cd openssl
# 这里需要了解openssl的配置信息
./config --prefix=/path/to/openssl -fPIC no-shared no-dso
make depend
make
make install
```

## 编译libcurl静态库

```
// download curl-7.87.0.tar.gz
// tar zxvf curl-7.87.0.tar
cd curl-7.87.0
./configure --prefix=/path/to/curl --disable-shared --enable-static --with-ssl=/path/to/openssl --without-libidn --without-librtmp  --without-nss --without-libssh2 --without-zlib --without-winidn --without-gnutls --disable-rtsp  --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-ldap

make
make install
```

## demo
在项目编译时, 需要添加头文件搜索路径&链接器需要搜索的路径

```
# 假设demo.c
gcc demo.c -I ./path/to/curl/include/ -I ./path/to/openssl/include/  -L ./path/to/curl/lib64/ -L ./path/to/openssl/ -lcurl -lssl -o demo
```