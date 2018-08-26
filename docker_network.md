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

https://segmentfault.com/a/1190000009491002


