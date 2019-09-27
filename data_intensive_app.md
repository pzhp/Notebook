
## TiDB:
- Partition
  Range, Hash(?)
- Replication/Sharding
  Raft
- Transaction:
  Yes
 
 ## MongoDB:
- Partition
  Range, Hash
- Replication/Sharding
  Raft
- Transaction:
  Yes (v4.0)

## Kafka
- Partition
  customized
- Replication/Sharding
  in-sync replica (follower pull)
- Transaction:
  Yes

## Redis
- Partition
  Hash slot: HASH_SLOT = CRC16(key) mod 16384
- Replication/Sharding
  asynchronous replication,  with asynchronous slave-to-master acknowledges of the amount of data processed
  [https://redis.io/topics/replication](https://redis.io/topics/replication)
- Transaction:
  Yes
## ElasticSearch
- Partition
  
- Replication/Sharding
  [https://www.elastic.co/cn/elasticon/conf/2017/sf/consensus-and-replication-in-elasticsearch](https://www.elastic.co/cn/elasticon/conf/2017/sf/consensus-and-replication-in-elasticsearch)
- Transaction:

## Cassandra
- Partition
  consistence hash
- Replication/Sharding
	choose sync/async, hinted handoff, read repair
- Transaction:
  
 ## HBase
  
