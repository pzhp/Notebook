## Get

mem table record => memtable key | memtable value | value size | value object

memtable key => varint32 key size | internal key

internal key =>  user key | key type | sequence num

**'|' means append**
- large sequence num is at front when user key is equal to find one time
- update is front than delete


https://blog.csdn.net/happylife1527/article/details/8333814

## format SST
SST = Data_Block | Meta_Block | Meta_Index_Block | Index_Block | Footer  (type & CRC)

### data & meta
Data_Block = Record_1 Record2 Record3 ... RecordN Restart[0] Restart[1] ... num_start  (prefix shared)

Meta_Block = Filter_1 ... FilterN Filter1_offset FilterN_offset  ... (one Filter per data block)

### index
Data_Index_Block: range (only keep max value), offset, size
Meta_Index_Block: key,offset,size   key is "filter." + name

### footer
Footer = index for  metaindex, index, padding, magic number








