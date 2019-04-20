## General
```
/sys/kernel/debug/tracing

a) tracer: function_graph  b) object: dev_queue_xmit c) option:
available_filter_functions记录了当前可以跟踪的内核函数
function =》 set_ftrace_filter 
function_graph =》set_grapch_function 
```

https://blog.csdn.net/pwl999/article/details/80627095 

## tracer: function_graph, trace callgraph including enter/return
```
trace-cmd record -p function_graph -g dev_queue_xmit -O funcgraph-proc -F  -- ssh localhost 22
```

## tracer: function, trace enter
```
trace-cmd record -p function -l dev_queue_xmit -F -- ssh localhost 22
cpus=1
             ssh-22911 [000] 3600228.234484: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.234503: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.234514: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.245838: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.248610: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.250998: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.259519: function:             dev_queue_xmit
             ssh-22911 [000] 3600228.298914: function:             dev_queue_xmit
```


