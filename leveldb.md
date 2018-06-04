#Get

mem table record => memtable key | memtable value | value size | value object
memtable key => varint32 key size | internal key
internal key =>  user key | key type | sequence num

**'|' means append**
- large sequence num is at front when user key is equal to find one time
- update is front than delete


https://blog.csdn.net/happylife1527/article/details/8333814
