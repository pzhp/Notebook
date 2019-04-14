**Docker Network**

1) 启动docker服务进程后， 会创建一个docker0 （朋友圈）， 
该host每启动一个container ， 会创建一个veth××， 并默认加入到docker0的朋友圈
```
// check by ifconfig
docker0: inet 172.17.0.1
// check ip address by docker inspect ${container_id}
container0: "IPAddress": "172.17.0.2"
container1: "IPAddress": "172.17.0.3"
container2: "IPAddress": "172.17.0.4"
```

2) 主机 iptables

```
// run one docker container with "IPAddress": "172.17.0.2"
[root@izwz92tr9v0fpakny88loaz ~]# docker container ls
CONTAINER ID        IMAGE                       COMMAND                  CREATED             STATUS              PORTS                                           NAMES
871a45d22e6b        dockersamples/static-site   "/bin/sh -c 'cd /u..."   3 days ago          Up 3 days           0.0.0.0:32769->80/tcp, 0.0.0.0:32768->443/tcp   static_site


[root@izwz92tr9v0fpakny88loaz ~]# iptables -L -v -n -t nat
Chain PREROUTING (policy ACCEPT 1 packets, 40 bytes)
 pkts bytes target     prot opt in     out     source               destination
 5613  268K DOCKER     all  --  *      *       0.0.0.0/0            0.0.0.0/0            ADDRTYPE match dst-type LOCAL

Chain INPUT (policy ACCEPT 1 packets, 40 bytes)
 pkts bytes target     prot opt in     out     source               destination

Chain OUTPUT (policy ACCEPT 3 packets, 228 bytes)
 pkts bytes target     prot opt in     out     source               destination
    0     0 DOCKER     all  --  *      *       0.0.0.0/0           !127.0.0.0/8          ADDRTYPE match dst-type LOCAL

Chain POSTROUTING (policy ACCEPT 3 packets, 228 bytes)
 pkts bytes target     prot opt in     out     source               destination
  121  6664 MASQUERADE  all  --  *      !docker0  172.17.0.0/16        0.0.0.0/0
    0     0 MASQUERADE  tcp  --  *      *       172.17.0.2           172.17.0.2           tcp dpt:443
    0     0 MASQUERADE  tcp  --  *      *       172.17.0.2           172.17.0.2           tcp dpt:80

Chain DOCKER (2 references)
 pkts bytes target     prot opt in     out     source               destination
    0     0 RETURN     all  --  docker0 *       0.0.0.0/0            0.0.0.0/0
    0     0 DNAT       tcp  --  !docker0 *       0.0.0.0/0            0.0.0.0/0            tcp dpt:32768 to:172.17.0.2:443
    0     0 DNAT       tcp  --  !docker0 *       0.0.0.0/0            0.0.0.0/0            tcp dpt:32769 to:172.17.0.2:80

// docker service 负载均衡采用类似的技术 https://www.jianshu.com/p/bbb673e79c3e 
```

3) 自定义一个bridge
```
// docker network create -d bridge mybridge
[root@izwz92tr9v0fpakny88loaz ~]# ifconfig
br-f975a4dce9f9: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        inet 172.19.0.1  netmask 255.255.0.0  broadcast 0.0.0.0
        ether 02:42:1c:c0:c1:c7  txqueuelen 0  (Ethernet)
        RX packets 28239  bytes 2633147 (2.5 MiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 28239  bytes 2633147 (2.5 MiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

// docker container connect mybridge ${container_id}
// docker inspect ${contianerid}, 
```
mybridge会出现在容器的bridge列表里，并且容器增加一个IPAddress:172.19.0.2， 
由于要分配一个的IP， 需要一个veth，这个ifconfig可查

```
            "Networks": {
                "bridge": {
                    "IPAMConfig": null,
                    "Links": null,
                    "Aliases": null,
                    "NetworkID": "5e4d141af1f3fe222dff460d9f852e2cec38ee0d434d79dca6c86ab33449cb9e",
                    "EndpointID": "946d0da4c3159e6d18b047c2b498e9c3c74afba715d92a30bc7a2602ded3a347",
                    "Gateway": "172.17.0.1",
                    "IPAddress": "172.17.0.2",
                    "IPPrefixLen": 16,
                    "IPv6Gateway": "",
                    "GlobalIPv6Address": "",
                    "GlobalIPv6PrefixLen": 0,
                    "MacAddress": "02:42:ac:11:00:02"
                },
                "mybridge": {
                    "IPAMConfig": {},
                    "Links": null,
                    "Aliases": [
                        "c813884c8a9b"
                    ],
                    "NetworkID": "c67699db7a13973e956c29ea5ece60e45298135798ff95ed24842447193ec23f",
                    "EndpointID": "120401191d35b3fc92e43ae03d57dccafec5686cf1c15b1c0446ccbbb33edd11",
                    "Gateway": "172.19.0.1",
                    "IPAddress": "172.19.0.2",
                    "IPPrefixLen": 16,
                    "IPv6Gateway": "",
                    "GlobalIPv6Address": "",
                    "GlobalIPv6PrefixLen": 0,
                    "MacAddress": "02:42:ac:14:00:02"
                }

```

4）相关命令和调试方法

**docker：**
```
docker run -dit alpine ash
docker  run -v docker-data:/docker-data -dit alpine ash
docker run -d -p 4000:80 friendlyhello 
docker tag <image> username/repository:tag
CTRL + p CTRL + q
```

**Linux cmd:**
ifconfig, ethtool, arp, route
iptables
perf trace --no-syscalls --event "net:*"
```
iptables -P INPUT DROP
iptables -A INPUT -s x.x.x.x -j DROP
iptables -t nat -A PREROUTING -p tcp -d 192.168.102.37 --dport 422 -j DNAT --to 192.168.102.37:22

route -n
[root@localhost zpeng]# route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.41.2    0.0.0.0         UG    100    0        0 ens33
192.168.2.0     0.0.0.0         255.255.255.0   U     0      0        0 veth0
192.168.41.0    0.0.0.0         255.255.255.0   U     100    0        0 ens33
192.168.122.0   0.0.0.0         255.255.255.0   U     0      0        0 virbr0

// bridge
# ip link add name bridge_name type bridge
# ip link set bridge_name up
# ip addr add dev bridge_name 192.168.66.66/24
# ip link set eth0 up
# ip link set eth0 master bridge_name
# bridge link show
This is how to remove an interface from a bridge:
# ip link set eth0 nomaster
# ip link set eth0 down
# ip link delete bridge_name type bridge
```

###perf trace###
```
[root@izwz92tr9v0fpakny88loaz ~]# perf trace --no-syscalls --event "net:*" ping 172.17.0.2 -c1 > /dev/null
     0.000 net:net_dev_queue:dev=docker0 skbaddr=0xffff88003b248900 len=98)
     0.011 net:net_dev_queue:dev=veth7233136 skbaddr=0xffff88003b248900 len=98)
     0.014 net:netif_rx:dev=eth0 skbaddr=0xffff88003b248900 len=84)
     0.017 net:net_dev_xmit:dev=veth7233136 skbaddr=0xffff88003b248900 len=98 rc=0)
     0.019 net:net_dev_xmit:dev=docker0 skbaddr=0xffff88003b248900 len=98 rc=0)
     0.022 net:netif_receive_skb:dev=eth0 skbaddr=0xffff88003b248900 len=84) // eth0 是容器里的接口
     
     0.043 net:net_dev_queue:dev=eth0 skbaddr=0xffff88003b248400 len=98)
     0.045 net:netif_rx:dev=veth7233136 skbaddr=0xffff88003b248400 len=84)
     0.047 net:net_dev_xmit:dev=eth0 skbaddr=0xffff88003b248400 len=98 rc=0)
     0.049 net:netif_receive_skb:dev=veth7233136 skbaddr=0xffff88003b248400 len=84)
     0.059 net:netif_receive_skb_entry:dev=docker0 napi_id=0x1 queue_mapping=0 skbaddr=0xffff88003b248400 vlan_tagged=0 vlan_proto=0x0000 vlan_tci=0x0000 protocol=0x0800 ip_summed=2 hash=0x00000000 l4_hash=0 len=84 data_len=0 truesize=768 mac_header_valid=1 mac_header=-14 nr_frags=0 gso_size=0 gso_type=0)
     0.061 net:netif_receive_skb:dev=docker0 skbaddr=0xffff88003b248400 len=84)

```

https://segmentfault.com/a/1190000009491002

https://www.cnblogs.com/CloudMan6/p/7096731.html


``` 
perf record -e net:net_dev_xmit -aR --call-graph dwarf  --  wget www.baidu.com
perf script

wget 30657 [000] 3110333.025779: net:net_dev_xmit: dev=eth0 skbaddr=0xffff98a2c1d63100 len=73 rc=0
            7fffb943ad99 dev_hard_start_xmit ([kernel.kallsyms])
            7fffb9467d6a sch_direct_xmit ([kernel.kallsyms])
            7fffb943dc61 __dev_queue_xmit ([kernel.kallsyms])
            7fffb943de30 dev_queue_xmit ([kernel.kallsyms])
            7fffb948a7b6 ip_finish_output ([kernel.kallsyms])
            7fffb948ad13 ip_output ([kernel.kallsyms])
            7fffb9488897 ip_local_out_sk ([kernel.kallsyms])
            7fffb948b776 ip_send_skb ([kernel.kallsyms])
            7fffb94b358c udp_send_skb ([kernel.kallsyms])
            7fffb94b4875 udp_sendmsg ([kernel.kallsyms])
            7fffb94c1f39 inet_sendmsg ([kernel.kallsyms])
            7fffb941b3a6 sock_sendmsg ([kernel.kallsyms])
            7fffb941bad1 SYSC_sendto ([kernel.kallsyms])
            7fffb941d5ee sys_sendto ([kernel.kallsyms])
            7fffb9576ddb system_call_fastpath ([kernel.kallsyms])
                   fee6d __libc_send (/usr/lib64/libc-2.17.so)
                    a170 __res_context_send (/usr/lib64/libresolv-2.17.so)
                    70a3 __GI___res_context_query (inlined)
                    7eef __GI___res_context_search (inlined)
                    2daa _nss_dns_gethostbyname4_r (/usr/lib64/libnss_dns-2.17.so)
                   e5793 gaih_inet.constprop.8 (/usr/lib64/libc-2.17.so)
                   e6b33 __GI_getaddrinfo (inlined)
                   144f7 [unknown] (/usr/bin/wget)
                   2f71f [unknown] (/usr/bin/wget)


```

```
ftrace to more 
trace-cmd record -p function_graph -g dev_queue_xmit -O funcgraph-proc -F  -- ssh localhost 22

[root@izwz92tr9v0fpakny88loaz zpeng]# trace-cmd report
cpus=1
             ssh-30928 [000] 3110612.717865: funcgraph_entry:                   |  dev_queue_xmit() {
             ssh-30928 [000] 3110612.717869: funcgraph_entry:                   |    __dev_queue_xmit() {
             ssh-30928 [000] 3110612.717869: funcgraph_entry:        0.172 us   |      local_bh_disable();
             ssh-30928 [000] 3110612.717870: funcgraph_entry:        0.206 us   |      netdev_pick_tx();
             ssh-30928 [000] 3110612.717871: funcgraph_entry:                   |      validate_xmit_skb.isra.105.part.106() {
             ssh-30928 [000] 3110612.717872: funcgraph_entry:                   |        netif_skb_features() {
             ssh-30928 [000] 3110612.717872: funcgraph_entry:        0.052 us   |          skb_network_protocol();
             ssh-30928 [000] 3110612.717873: funcgraph_exit:         0.794 us   |        }
             ssh-30928 [000] 3110612.717873: funcgraph_entry:        0.050 us   |        skb_csum_hwoffload_help();
             ssh-30928 [000] 3110612.717874: funcgraph_exit:         1.923 us   |      }
             ssh-30928 [000] 3110612.717874: funcgraph_entry:                   |      dev_hard_start_xmit() {
             ssh-30928 [000] 3110612.717875: funcgraph_entry:                   |        loopback_xmit() {
             ssh-30928 [000] 3110612.717875: funcgraph_entry:                   |          tcp_wfree() {
             ssh-30928 [000] 3110612.717876: funcgraph_entry:        0.059 us   |            sock_wfree();
             ssh-30928 [000] 3110612.717876: funcgraph_exit:         0.493 us   |          }
             ssh-30928 [000] 3110612.717876: funcgraph_entry:        0.600 us   |          eth_type_trans();
             ssh-30928 [000] 3110612.717877: funcgraph_entry:                   |          netif_rx() {
             ssh-30928 [000] 3110612.717877: funcgraph_entry:                   |            netif_rx_internal() {
             ssh-30928 [000] 3110612.717878: funcgraph_entry:                   |              ktime_get_real() {
             ssh-30928 [000] 3110612.717878: funcgraph_entry:                   |                getnstimeofday64() {
             ssh-30928 [000] 3110612.717878: funcgraph_entry:        0.145 us   |                  __getnstimeofday64();
             ssh-30928 [000] 3110612.717879: funcgraph_exit:         0.474 us   |                }
             ssh-30928 [000] 3110612.717879: funcgraph_exit:         0.951 us   |              }
             ssh-30928 [000] 3110612.717879: funcgraph_entry:        0.256 us   |              get_rps_cpu();
             ssh-30928 [000] 3110612.717880: funcgraph_entry:                   |              enqueue_to_backlog() {
             ssh-30928 [000] 3110612.717880: funcgraph_entry:        0.057 us   |                _raw_qspin_lock();
             ssh-30928 [000] 3110612.717880: funcgraph_entry:        0.065 us   |                __raise_softirq_irqoff();
             ssh-30928 [000] 3110612.717881: funcgraph_exit:         1.121 us   |              }
             ssh-30928 [000] 3110612.717881: funcgraph_exit:         3.493 us   |            }
             ssh-30928 [000] 3110612.717882: funcgraph_exit:         4.093 us   |          }
             ssh-30928 [000] 3110612.717882: funcgraph_exit:         6.994 us   |        }
             ssh-30928 [000] 3110612.717882: funcgraph_exit:         7.956 us   |      }
             ssh-30928 [000] 3110612.717882: funcgraph_entry:                   |      local_bh_enable() {
             ssh-30928 [000] 3110612.717883: funcgraph_entry:        0.083 us   |        __local_bh_enable_ip();
             ssh-30928 [000] 3110612.717883: funcgraph_exit:         0.562 us   |      }
             ssh-30928 [000] 3110612.717883: funcgraph_exit:       + 14.572 us  |    }
             ssh-30928 [000] 3110612.717884: funcgraph_exit:       + 15.500 us  |  }


