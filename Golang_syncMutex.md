
|  Lock   | Linux Kernel  | Golang |  pthread |
|  ----   | ----          | ----   | ----     |
| SpinLock|  | | |
| Mutex   |  | | |
| RWLock  |  | | |
| Sem     |  | | |


```
1. Ticket
2. CLHLock
CLH锁是一种基于链表的可扩展、高性能、公平的自旋锁，申请线程只在本地变量上自旋，它不断轮询前驱的状态，如果发现前驱释放了锁就结束自旋，获得锁。
CLHLock是通过轮询其前驱节点的状态

3. MCSLock
https://zhuanlan.zhihu.com/p/89058726
MCSLock则是对本地变量的节点进行循环。
MCS则是查看当前节点的锁状态。
MCS spin lock

4. qspinlock
https://zhuanlan.zhihu.com/p/100546935 

5. osq lock: combined MCS lock and qspinlock for mutex/rwsem 
https://zhuanlan.zhihu.com/p/90508284
fast path:
mid path [spnner queue]: spnner, assume previous ownert release lock fast
slow path [waiter queue]: waiter

5. 其他
	Mutex per core
		one represent per core
	NUMA Aware 
```


多个goroutine同时等在一个Mutex上，Lock/Unlock, 本质上： Mutex-> Sema ->futexwakeup/futexsleep

```
                                 ┌──────────┐                                    
                            ┌─┬─▶│  sudog   │                                    
                            │ │  ├──────────┴────────────┐                       
      ┌─────────────────────┼─┼──│      prev *sudog      │                       
      │                     │ │  ├───────────────────────┤                       
      │                     │ │  │      next *sudog      │────┐                  
      │                     │ │  ├───────────────────────┤    │                  
      │                     │ │  │     parent *sudog     │    │                  
      │                     │ │  ├───────────────────────┤    │                  
      │                     │ │  │  elem unsafe.Pointer  │    │                  
      │                     │ │  ├───────────────────────┤    │                  
      │                     │ │  │     ticket uint32     │    │                  
      │                     │ │  └───────────────────────┘    │                  
      │                     │ │                               │                              
      │                     │ │                               │                  
      │                     │ │                               │                  
      │                     │ │                               │                  
      ▼                     │ │                               ▼                  
┌──────────┐                │ │                         ┌──────────┐             
│  sudog   │                │ │                         │  sudog   │             
├──────────┴────────────┐   │ │                         ├──────────┴────────────┐
│      prev *sudog      │   │ │                         │      prev *sudog      │
├───────────────────────┤   │ │                         ├───────────────────────┤
│      next *sudog      │   │ │                         │      next *sudog      │
├───────────────────────┤   │ │                         ├───────────────────────┤
│     parent *sudog     │───┘ └─────────────────────────│     parent *sudog     │
├───────────────────────┤                               ├───────────────────────┤
│  elem unsafe.Pointer  │                               │  elem unsafe.Pointer  │
├───────────────────────┤                               ├───────────────────────┤
│     ticket uint32     │                               │     ticket uint32     │
└───────────────────────┘                               └───────────────────────┘


┌──────────┐                         ┌──────────┐                          ┌──────────┐             
│  sudog   │                  ┌─────▶│  sudog   │                   ┌─────▶│  sudog   │             
├──────────┴────────────┐     │      ├──────────┴────────────┐      │      ├──────────┴────────────┐
│    waitlink *sudog    │─────┘      │    waitlink *sudog    │──────┘      │    waitlink *sudog    │
├───────────────────────┤            ├───────────────────────┤             ├───────────────────────┤
│    waittail *sudog    │            │    waittail *sudog    │             │    waittail *sudog    │
└───────────────────────┘            └───────────────────────┘             └───────────────────────┘
```


``` golang


// A semaRoot holds a balanced tree of sudog with distinct addresses (s.elem).
// Each of those sudog may in turn point (through s.waitlink) to a list
// of other sudogs waiting on the same address.
// The operations on the inner lists of sudogs with the same address
// are all O(1). The scanning of the top-level semaRoot list is O(log n),
// where n is the number of distinct addresses with goroutines blocked
// on them that hash to the given semaRoot.
type semaRoot struct {
	lock  mutex
	treap *sudog // root of balanced tree of unique waiters.
	nwait uint32 // Number of waiters. Read w/o the lock.
}

// Prime to not correlate with any user patterns.
const semTabSize = 251

var semtable [semTabSize]struct {
	root semaRoot
	pad  [cpu.CacheLinePadSize - unsafe.Sizeof(semaRoot{})]byte
}


type sudog struct {
	g *g

  // construct treap:  prev -> left node,  next-> right node
	next *sudog
	prev *sudog
  
	elem unsafe.Pointer

	acquiretime int64
	releasetime int64
	ticket      uint32  // keep same for one group

	isSelect bool
	success bool
  
  // construct treap
	parent   *sudog // semaRoot binary tree
  
  // construct single link list: 
  // case 1: S->nil  
  // case 2: S->S1->S2->nil
  //      dequeue: remove S from list, adjust list and treap
  //      queue: add to list head or tail based on bool lifo
	waitlink *sudog // next sudog
	waittail *sudog // fast locate tailing to add new sudog
  
	c        *hchan // channel
}

```


``` C++
// Linux:
// spinlock
// spin_lock:     disable_preempt
// spin_lock_bh:  disable software interrupt
// spin_lock_irq: disable hardware interrupt
// spin_lock_irqsave: // lock1; lock2; unlock2; trigger clear disable hardware interrupt, so need it

typedef struct qspinlock {
    union {
        atomic_t val;

        struct {
            u8    locked;
            u8    pending;
        };
        struct {
            u16    locked_pending;
            u16    tail;
        };
    };
} arch_spinlock_t;


struct semaphore {
	raw_spinlock_t		lock;
	unsigned int		count;
	struct list_head	wait_list;
};
https://blog.csdn.net/weixin_41177620/article/details/90735287


/*
 * - only one task can hold the mutex at a time
 * - only the owner can unlock the mutex
 * - multiple unlocks are not permitted
 * - recursive locking is not permitted
 * ....
 * /
// Linux v5.13
struct mutex {
	atomic_long_t		owner;
	spinlock_t		wait_lock;
#ifdef CONFIG_MUTEX_SPIN_ON_OWNER
	struct optimistic_spin_queue osq; /* Spinner MCS lock */
#endif
	struct list_head	wait_list;
#ifdef CONFIG_DEBUG_MUTEXES
	void			*magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
};

```


