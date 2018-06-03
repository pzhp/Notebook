
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

```


-- others
 perf_events Off-CPU Time Flame Graph
 `pgrep -nx tar`

 perf script -i perf.data | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > perf.svg

 如果sleep， 普通模式捕获不到sample


 CPU Statistics  "perf stat"
 Timed Profiling "perf record -F 99 -a -g -- sleep 30"
 Event Profiling "perf record -e L1-dcache-load-misses -c 10000 -ag -- sleep 5"
 Dynamic Tracing “perf probe --add tcp_sendmsg”  
    perf probe --del  
    perf probe -V tcp_sendmsg   （var）
    perf probe -L tcp_sendmsg  （line）
    perf probe --add 'tcp_sendmsg size' // var
    perf probe --add 'tcp_sendmsg:81 seglen' // line

    perf probe -x /lib/x86_64-linux-gnu/libc-2.15.so --add malloc

perf report --sort 
--
perf record -e L1-dcache-load-misses -c 10000 -ag -- sleep 5

perf record -e probe_a:beg  -e probe_a:end -aR sleep 20

 -- off CPU
 # perf record -e sched:sched_stat_sleep -e sched:sched_switch \
    -e sched:sched_process_exit -a -g -o perf.data.raw sleep 1
# perf inject -v -s -i perf.data.raw -o perf.data
# perf script -f comm,pid,tid,cpu,time,period,event,ip,sym,dso,trace | awk '
    NF > 4 { exec = $1; period_ms = int($5 / 1000000) }
    NF > 1 && NF <= 4 && period_ms > 0 { print $2 }
    NF < 2 && period_ms > 0 { printf "%s\n%d\n\n", exec, period_ms }' | \
    ./stackcollapse.pl | \
    ./flamegraph.pl --countname=ms --title="Off-CPU Time Flame Graph" --colors=io > offcpu.svg

# echo 1 > /proc/sys/kernel/sched_schedstats
