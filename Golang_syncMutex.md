
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
struct mutex {
	/* 1: unlocked, 0: locked, negative: locked, possible waiters */
	atomic_t		count;
	spinlock_t		wait_lock;
	struct list_head	wait_list;
#if defined(CONFIG_DEBUG_MUTEXES) || defined(CONFIG_SMP)
	struct thread_info	*owner;
#endif
#ifdef CONFIG_DEBUG_MUTEXES
	const char 		*name;
	void			*magic;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
};

```

``` semaphore: Linux kernel
struct semaphore {
    raw_spinlock_t        lock;
    unsigned int        count;
    struct list_head    wait_list; // put waiting task in the list
};

// wrapper for a task
struct semaphore_waiter {
	struct list_head list;
	struct task_struct *task;
	bool up;
};
```

