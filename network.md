## route

```
fib_lookup()


FIB table
struct fib_table{};
struct fib_info{};
struct fib_alias{};
struct fib_nh{}; // next hop

struct rtable{}; // 一条路由
struct dst_entry{};
```

```
wget  8901 [000] 3620158.928211: probe:dev_hard_start_xmit: (ffffffffb9438aa0)
                  838aa1 dev_hard_start_xmit (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  83bc61 __dev_queue_xmit (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  83be30 dev_queue_xmit (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)  ** neigh->output() **
                  8887b6 ip_finish_output (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  888d13 ip_output (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  886897 ip_local_out_sk (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  889776 ip_send_skb (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  8b158c udp_send_skb (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  8b2875 udp_sendmsg (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  8bff39 inet_sendmsg (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  8193a6 sock_sendmsg (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  819ad1 SYSC_sendto (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  81b5ee sys_sendto (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                  974ddb system_call (/usr/lib/debug/lib/modules/3.10.0-957.1.3.el7.x86_64/vmlinux)
                   fee6d __libc_send (/usr/lib64/libc-2.17.so)
```
