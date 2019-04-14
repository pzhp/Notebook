#1 print function graph
```
trace-cmd record -p function_graph -g dev_queue_xmit -O funcgraph-proc -F  -- ssh localhost 22
a) tracer: function_graph  b) object: dev_queue_xmit c) option:

available_filter_functions记录了当前可以跟踪的内核函数
function =》 set_ftrace_filter 
function_graph =》set_grapch_function 
```
