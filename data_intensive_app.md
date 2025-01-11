
## TiDB:
- shared-nothing
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
## Apache Pulsar
- 
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
- data model from Bigtable, distributed design from DynamoDB - wide-column storage
- decentralized(gossip)， masterless architecture
- Partition
  consistence hash
- Replication/Sharding
	choose sync/async, hinted handoff, read repair
- Transaction:
  
## HBase(CAP:CP)
- Strongly consistent reads/writes

## DynamoDB
- DynamoDB uses the Paxos algorithm to elect leaders.
- DynamoDb离线检查采用的Merkle Tree
- DynamoDB uses hashing and B-trees to manage data. Upon entry, data is first distributed into different partitions by hashing on the partition key. 
- Dynamo is based on leaderless replication, DynamoDB uses single-leader replication

## Dynamo
|               Problem              |                                Technique                                |                                                    Advantage                                                    |
|:----------------------------------:|:-----------------------------------------------------------------------:|:---------------------------------------------------------------------------------------------------------------:|
| Dataset partitioning               | Consistent Hashing                                                      | Incremental, possibly linear scalability in proportion to the number of collaborating nodes.                    |
| Highly available writes            | Vector Clock or Dotted-Version-Vector Sets, reconciliation during reads | Version size is decoupled from update rates.                                                                    |
| Handling temporary failures        | Sloppy Quorum and Hinted Handoff                                        | Provides high availability and durability guarantee when some of the replicas are not available.                |
| Recovering from permanent failures | Anti-entropy using Merkle tree                                          | Can be used to identify differences between replica owners and synchronize divergent replicas pro-actively.     |
| Membership and failure detection   | Gossip-based membership protocol and failure detection                  | Avoids having a centralized registry for storing membership and node liveness information, preserving symmetry. |

## Amazon Aurora
- multimaster architure
 	Aurora reports a write conflict to your application as a deadlock error. This error condition causes the transaction to roll back. 
	If two DB instances attempt to modify the same data page at almost the same instant, a write conflict occurs. The earliest change request is approved using a quorum voting mechanism. That change is saved to permanent storage. The DB instance whose change isn't approved rolls back the entire transaction containing the attempted change. 
- shared-disk
  
## Snowflake
- meta data FoundationDB
- hybrid of traditional shared-disk（data-> cloud） and shared-nothing database architectures (compute only partial data)

## Bigtable
- single master to keep strong consistency
## Spanner
- TrueTime -> to keep strong consistency
- Paxos

## F1
- Based on Spanner
- *Schema change*: absent --> delete only --> write only --(reorg)--> public   (TiDB also use it)

## ClickHouse
- column-oriented, OLAP 
- shard
- replication
- no transaction
