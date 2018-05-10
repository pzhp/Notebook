## Outline
- read/write
- iterator
- compaction
- memtable, sst/manifest/log format
- transaction: snapshot, 


## Transaction
TransactionDB or OptimisticTransactionDB

By default, Transaction conflict checking validates that no one else has written a key after the time the key was first written in this transaction. 
This isolation guarantee is sufficient for many use-cases. 
However, you may want to guarantee that no else has written a key since the start of the transaction. 
This can be accomplished by calling SetSnapshot() after creating the transaction.
// write-write conflict, read-write conflict
txn->SetSnapshot();
read_options.snapshot = txn->GetSnapshot();
Status s = txn->GetForUpdate(read_options, “key1”, &value); // conflict validate

WriteBatch => keep atomic
Transactions  => write when no conflict

ref https://zhuanlan.zhihu.com/p/31255678 
