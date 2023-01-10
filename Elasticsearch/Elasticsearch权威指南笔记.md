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