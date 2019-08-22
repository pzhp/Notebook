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

## Paxos
P1. An acceptor must accept the first proposal that it receives.

P2. If a proposal with value v is chosen, then every higher-numbered proposal
that is chosen has value v.
P2a. If a proposal with value v is chosen, then every higher-numbered proposal
accepted by any acceptor has value v.
P2b. If a proposal with value v is chosen, then every higher-numbered proposal
issued by any proposer has value v.
P2c. For any v and n, if a proposal with value v and number n is issued,
then there is a set S consisting of a majority of acceptors such that
either (a) no acceptor in S has accepted any proposal numbered less
than n, or (b) v is the value of the highest-numbered proposal among
all proposals numbered less than n accepted by the acceptors in S.

P1a. An acceptor can accept a proposal numbered n iff it has not responded
to a prepare request having a number greater than n

/// Phase
Phase 1. (a) A proposer selects a proposal number n and sends a prepare
request with number n to a majority of acceptors.
(b) If an acceptor receives a prepare request with number n greater
than that of any prepare request to which it has already responded,
then it responds to the request with a promise not to accept any more
proposals numbered less than n and with the highest-numbered proposal
(if any) that it has accepted.

Phase 2. (a) If the proposer receives a response to its prepare requests
(numbered n) from a majority of acceptors, then it sends an accept
request to each of those acceptors for a proposal numbered n with a
value v, where v is the value of the highest-numbered proposal among
the responses, or is any value if the responses reported no proposals.
(b) If an acceptor receives an accept request for a proposal numbered
n, it accepts the proposal unless it has already responded to a prepare
request having a number greater than n

// learner failed
Because of message loss, a value could be chosen with no learner ever
finding out. The learner could ask the acceptors what proposals they have
accepted, but failure of an acceptor could make it impossible to know whether
or not a majority had accepted a particular proposal. In that case, learners
will find out what value is chosen only when a new proposal is chosen. If
a learner needs to know whether a value has been chosen, it can have a
proposer issue a proposal, using the algorithm described abovelearn

// leader can propose


## isolation
### 
https://medium.com/@chester.yw.chu/%E5%B0%8D%E6%96%BC-mysql-repeatable-read-isolation-%E5%B8%B8%E8%A6%8B%E7%9A%84%E4%B8%89%E5%80%8B%E8%AA%A4%E8%A7%A3-7a9afbac65af
這就像對資料庫做了一個 Snapshot 一樣，Transaction 只能看到這個 Snapshot 的內容，但是無法讀取到其他 Transaction 所做的更新。但是在 InnoDB 的實作中，這個規則只限於 SELECT (DQL) 指令，其他像是 INSERT、UPDATE 和 DELETE 等 DML 指令，看到的就不是 Snapshot，而是指令執行當下所有已經被 commit 的資料。所以在上面的例子中，Transaction 在進行 UPDATE 指令時，看到的就是資料庫當下真實的資料，所有已經被 Commit 的資料都包含在內。這也就是為什麼 Transaction A 在執行 UPDATE 時可以看到玩家 Frank，並且幫他增加 credit。而且在執行完 UPDATE 後，重新 SELECT 一次時，玩家 Frank 也出現在列表中 (Transaction 可以看到自己所做的更新)。
同樣是採用 Snapshot Isolation 實作 RR Isolation 的 PostgreSQL，它的 Snapshot 就不只在 SELECT 指令有效，其他像 INSERT、UPDATE 和 DELETE 等 DML 指令上也都有效。所以上面例子中的 Phantom 現象並不會在 PostgreSQL 發生。


- 誤解 1：MySQL Repeatable Read Isolation 可以避免 Phantom   (PostgreSQL RR can avoid Phantom)
- 誤解 2：Repeatable Read Isolation 不會有 Lost Update (PostgreSQL RR can avoid Lost Update)
- 誤解 3：MySQL Range Locks on Indexed and Non-indexed Column

###
“In PostgreSQL, you can request any of the four standard transaction isolation levels. But
internally, there are only three distinct isolation levels, which correspond to the levels Read
Committed, Repeatable Read, and Serializable. When you select the level “Read Uncommitted”
you really get “Read Committed”, and phantom reads are not possible in the PostgreSQL
implementation of “Repeatable Read”, so the actual isolation level might be stricter than what
you select. This is permitted by the SQL standard: the four isolation levels only define which
phenomena must not happen, they do not define which phenomena must happen. The reason
that PostgreSQL only provides three isolation levels is that this is the only sensible way to map
the standard isolation levels to the multiversion concurrency control architecture.”

https://wiki.hsr.ch/Datenbanken/files/Paper_ANSI_SQL_Isolation_Levels_Stefan_Luetolf_V2_1.pdf



