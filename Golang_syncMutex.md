
多个goroutine同时等在一个Mutex上，Lock/Unlock的流程：


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

