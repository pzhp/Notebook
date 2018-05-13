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
