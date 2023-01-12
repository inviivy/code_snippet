# 安装

```
git clone https://github.com/curl/curl.git --depth=1

// download curl-7.87.0.tar.gz
// tar zxvf curl-7.87.0.tar

cd curl-7.87.0
sudo apt install openssl
# enable ssl
./configure --with-openssl
make
sudo make install
```