
``` c
struct epitem {

    struct rb_node  rbn;        //用于主结构管理的红黑树

    struct list_head  rdllink;  //事件就绪队列

    struct epitem  *next;       //用于主结构体中的链表

    struct epoll_filefd  ffd;   //这个结构体对应的被监听的文件描述符信息

    int  nwait;                 //poll操作中事件的个数

    // list_add_tail(&pwq->llink, &epi->pwqlist);
    // struct eppoll_entry *pwq;
    struct list_head  pwqlist;  // 双向链表，保存着被监视文件的等待队列，功能类似于select/poll中的poll_table

    struct eventpoll  *ep;      //该项属于哪个主结构体（多个epitm从属于一个eventpoll）

    // static inline void list_add_tail(struct list_head *new, struct list_head *head)
    struct list_head  fllink;   // list_add_tail(&epi->fllink,&tfile->f_ep_lilnks);


    struct epoll_event  event;  //注册的感兴趣的事件,也就是用户空间的epoll_event
}

// epoll (wait queue) => epoll_item => fd (wait queue)

static void ep_ptable_queue_proc(struct file *file, wait_queue_head_t *whead,poll_table *pt);
static int ep_poll_callback(wait_queue_t *wait, unsigned mode, int sync, void *key);
```
