
ftrace
perf
eBPF
BCC
sysdig = strace + tcpdump + htop + iftop + lsof + docker inspect




measure duration
```bash
#!/bin/bash

bin="$1"
beg="$2"
end="$3"


bin_name=${bin##*/}
bin_name=probe_${bin_name%%.*}
beg_name=`echo ${beg} | sed 's/[.:]/_/g'`
end_name=`echo ${end} | sed 's/[.:]/_/g'`

echo '*********************** ' 
echo "${bin_name} , " "${beg_name} , " "${end_name}"


perf probe --del ${bin_name}:${beg_name}*
perf probe --del ${bin_name}:${end_name}*


echo '*********************** ' 
perf probe -x ${bin} -f --add ${beg_name}=${beg}
perf probe -x ${bin} -f --add ${end_name}=${end}


echo '*********************** ' 
echo "probes added:"
perf probe -l


echo '*********************** ' 
echo "begin measure"
perf record -e ${bin_name}:${beg_name} -e ${bin_name}:${end_name} -a sleep 10

##Usage
perf probe -x a.out --line main
./perf.sh a.out main:4 main:20
perf script
           a.out  7715 [000] 3120451.918413:  probe_a:main_4: (400aac)
           a.out  7715 [000] 3120452.918818: probe_a:main_20: (400b76)


```


-- others

```
 perf script -i perf.data | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > perf.svg

 CPU Statistics  "perf stat"
 Timed Profiling "perf record -F 99 -a -g -- sleep 30"
 Event Profiling "perf record -e L1-dcache-load-misses -c 10000 -ag -- sleep 5"
 Trace : perf trace ls
 Dynamic Tracing “perf probe --add tcp_sendmsg”  
    perf probe --del  
    perf probe -V tcp_sendmsg   （var）
    perf probe -L tcp_sendmsg  （line）
    perf probe --add 'tcp_sendmsg size' // var
    perf probe --add 'tcp_sendmsg:81 seglen' // line

    perf probe -x /lib/x86_64-linux-gnu/libc-2.15.so --add malloc



perf record -e L1-dcache-load-misses -c 10000 -ag -- sleep 5
perf record -e probe_a:beg  -e probe_a:end -aR sleep 20

perf report --sort 
```

## concept:

int perf_event_open(struct perf_event_attr *attr,pid_t pid, int cpu, int group_fd,
                    unsigned long flags);

pid == 0: if the pid parameter is zero, the counter is attached to the
current task.
pid > 0: the counter is attached to a specific task (if the current task
has sufficient privilege to do so)
pid < 0: all tasks are counted (per cpu counters)

cpu >= 0: the counter is restricted to a specific CPU
cpu == -1: the counter counts on all CPUs
(Note: the combination of 'pid == -1' and 'cpu == -1' is not valid. **why**)

A 'pid > 0' and 'cpu == -1' counter is a per task counter that counts
events of that task and 'follows' that task to whatever CPU the task
gets schedule to. Per task counters can be created by any user, for
their own tasks.

A 'pid == -1' and 'cpu == x' counter is a per CPU counter that counts
all events on CPU-x. Per CPU counters need CAP_SYS_ADMIN privilege.

