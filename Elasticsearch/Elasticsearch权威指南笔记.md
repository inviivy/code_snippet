# Elasticsearch权威指南

## 第1章 快速入门

Elasticsearch使用场景:

+ 我们经营某个网店，允许客户搜索我们所销售的产品。这个时候可以使用Elasticsearch存储整个产品目录和库存，且提供搜索和搜索词自动补全的功能。
+ 希望收集日志或者事务数据，并且分析其中的趋势、统计特性、摘要或反常现象。=> 使用Logstash。一旦进入Elasticsearch，可以通过搜索和聚合来挖掘感兴趣的任何信息
+ 价格报警平台。=> 类似什么值得买或者比价工具这样的东西?
+ 分析或商业只能需求和构建自定义表盘以可视化数据 => 类似地比如clickhouse等

### 1.1 基本概念

+ 近实时(Near Real Time, NRT)搜索和分析平台 => 从索引文档到可搜索文档会有一段小的延迟
+ 集群 => 唯一名称标识(elasticsearch.yml配置cluster.name) => 节点只能通过集群名称加入集群(这些节点将共同拥有完整的数据，并跨节点提供联合索引、搜索和分析功能)
+ 节点 => elasticsearch的运行实例, 也就是一个进程，服务发现=>强调自动加入集群，一台服务器可以拥有多个节点（因为可以有多个进程）。
+ 单个集群可以拥有任意多个节点
+ 索引 => 是具有某种相似特性的文档集合。索引由一个名称（必须全部是小写）标识，当对其中的文档执行索引、搜索、更新和删除操作时，该名称指向这个特定的索引
+ 文档 => 文档（document）是可以被索引的基本信息单元。
+ 分片和副本 => 索引可能会存储大量数据，这些数据可能会超出单个节点的硬件限制。副本的高可用性

### 1.2 安装部署

```shell
wget -qO - https://artifacts.elastic.co/GPG-KEY-elasticsearch | sudo gpg --dearmor -o /usr/share/keyrings/elasticsearch-keyring.gpg
sudo apt-get install apt-transport-https
echo "deb [signed-by=/usr/share/keyrings/elasticsearch-keyring.gpg] https://artifacts.elastic.co/packages/8.x/apt stable main" | sudo tee /etc/apt/sources.list.d/elastic-8.x.list
sudo apt-get update && sudo apt-get install elasticsearch

# 密码重置
sudo /usr/share/elasticsearch/bin/elasticsearch-reset-password -u elastic
# 验证是否启动
curl --cacert /etc/elasticsearch/certs/http_ca.crt -u elastic https://localhost:9200

sudo systemctl daemon-reload
sudo systemctl enable elasticsearch.service
sudo systemctl start elasticsearch.service


# 或者直接下载二进制
wget https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-8.5.3-amd64.deb
wget https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-8.5.3-amd64.deb.sha512
shasum -a 512 -c elasticsearch-8.5.3-amd64.deb.sha512 
sudo dpkg -i elasticsearch-8.5.3-amd64.deb
```
#### reference
+ [elasticsearch install](https://www.elastic.co/guide/en/elasticsearch/reference/8.5/deb.html#deb-repo)

### 1.3 使用集群

+ 检查集群的运行情况: ```sudo curl --cacert /etc/elasticsearch/certs/http_ca.crt -u elastic -XGET https://localhost:9200/_cat/health?v```, 绿色/黄色/红色
+ 查看节点信息: ```GET /_cat/nodes?v```
+ 查看索引信息: ```GET /_cat/indices?v```
+ 创建索引: ```PUT /customer?v```
+ 添加文档: 
```
sudo curl --cacert /etc/elasticsearch/certs/http_ca.crt -u elastic -XPUT "https://localhost:9200/customer/_doc/1?pretty" -H 'Content-Type: application/json' -d'
{
  "name": "John"
}
'
```
+ 搜索文档: ```/GET customer/_doc/1?pretty```
+ 更新or覆盖文档: ```/PUT customer/_doc/1?pretty -H 'Content-Type: application/json' -d'{}'```
+ 批量增加/删除文档
+ 更新文档post
+ 删除文档delete

### 1.4 修改数据

### 1.5 搜索数据
很多情况下拥有和sql相同的语义.

elasticsearch提供了3个接口:
+ REST api
+ 基于url语义的api: ```?xxA=xxA&xxb=xxb&pretty```
+ 基于json语义的api: GET方法中带一个json的请求语义(本质上是一个DSL)

```
sudo curl --cacert /etc/elasticsearch/certs/http_ca.crt -u elastic -XGET https://localhost:9200/customer/_search -H 'Content-Type: application/json' -d'
{
  "query": {
    "match_all": {}
  },
  "_source": []
}
'
```

#### 查询语义
+ 返回特定字段
+ 匹配查询: ```{"match": {"字段": "必须包含的string"}}```
+ 布尔查询：```"bool" : {"must" : [...]}```
+ 聚合查询(group by语义): ```"aggs"```

## 第2章 安装和部署

### 2.2 安装Elasticsearch

调整Linux系统的相关参数设置:

+ 修改最大文件数和锁内存限制: /etc/security/limits.conf
+ 更改一个进程能拥有的最大内存区域限制: /etc/sysctl.conf
+ 修改用户最大线程数: /etc/security/limits.d/90-nproc.conf

#### reference
+ [linux-unix系统编程手册第11章]

#### 2.5.5 重要节点发现和集群初始化设置

关于发现应该参考p2p相关的技术?或者分布式技术?

##### 1. 种子列表
没有任何网络配置的情况下, Elasticsearch将绑定到可用的环回地址, 并扫描本地的端口9300到9305, 以尝试连接到同一服务器上运行的其他节点. 这提供了一种自动组建集群的功能，而无须进行任何配置.

当要与其他主机上的节点组成集群时, 必须使用discovery.seed_hosts设置提供集群中其他节点的列表, 这些节点可以是活动的和可通信的, 以便为发现过程设定种子. 通常形式为host:port.

##### 2. 候选主节点列表
当第一次启动一个全新的Elasticsearch集群时, 有一个集群引导过程, 它确定在第一次主节点选举中计票的合格主节点集.

### 2.9 集群水平扩展
当向集群添加更多节点时，它会自动分配分片和副本.

## 第3章 api规范
http协议&json格式

### 3.1 多索引
支持跨多个索引&通配符(但是不知道是否支持正则)

### 3.2 日期数学格式

### 3.3 通用选项

#### 3.3.1 格式化搜索结果
格式化搜索结果
pretty

+ 3.3.2 可读输出
+ 3.3.3 格式化日期值
+ 3.3.4 返回信息过滤
+ 3.3.5 展开设置
+ 3.3.6 布尔值
+ 3.3.7 数字值
+ 3.3.8 时间单位
+ 3.3.9 数据单位
+ 3.3.10 缩略处理
+ 3.3.12 模糊性
+ 3.3.14 查询字符串中的请求正文
+ 3.3.15 Content-Type要求

### 3.4 基于URL的访问控制

## 第4章 操作文档

单文档:

+ Index API
+ Get API
+ Delete API
+ Update API

多文档:

+ Multi Get API
+ Bulk API
+ Delete By Query API
+ Update By Query API
+ Reindex API

### 4.1 读写文档
Elasticsearch的数据复制是基于主备(primary-backup)模型, Pacifica论文中有很好的描述.

#### 4.1.1 基本写模型

#### 4.1.2 写流程错误处理

#### 4.1.3 基本读模型

#### 4.1.4 读流程错误处理

## 4.2 索引API

### 4.2.2 ID自动生成

### 4.2.3 路由

### 4.2.4 分发

## 4.3 GET API
