
## TiDB:
- Partition  
  Range, Hash(?)
- Replication  
  Raft
- Transaction:  
  Yes
 
## MongoDB:
- Partition  
  Range, Hash
- Replication  
  Raft
- Transaction:  
  Yes (v4.0)

## Etcd:
- Partition  
- Replication  
  Raft
- Transaction:  

## Kafka
- Partition  
  customized
- Replication  
  in-sync replica (follower pull)
- Transaction:
  Yes

## Redis
- Partition  
  Hash slot: HASH_SLOT = CRC16(key) mod 16384
- Replication  
  asynchronous replication,  with asynchronous slave-to-master acknowledges of the amount of data processed
  [https://redis.io/topics/replication](https://redis.io/topics/replication)
- Transaction:  
  Yes
## ElasticSearch
Cluster coordination model vs Data replication model  
- Partition  
 
- Replication  
  ESV7 introduce new consensus algorithm, before that it is zen discovery
  
  **before 6.0.0**  
  [https://www.elastic.co/cn/elasticon/conf/2017/sf/consensus-and-replication-in-elasticsearch](https://www.elastic.co/cn/elasticon/conf/2017/sf/consensus-and-replication-in-elasticsearch)  
  
  **6.0.0** introduce sequence number based data replication approach  
  	- local checkpoint, global checkpoint
- Transaction:

## Cassandra(CAP:AP)
- data model from Bigtable, distributed design from DynamoDB
- decentralized(gossip), 
- Partition
  consistence hash
- Replication/Sharding
	choose sync/async, hinted handoff, read repair
- Transaction:
  
## HBase(CAP:CP)
- Strongly consistent reads/writes

## DynamoDB
- DynamoDb离线检查采用的Merkle Tree
  
