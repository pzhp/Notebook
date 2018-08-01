## terminal

```
Ctrl + A	Go to the beginning of the line you are currently typing on
Ctrl + E	Go to the end of the line you are currently typing on
Ctrl + L              	Clears the Screen, similar to the clear command
	
Ctrl + R	Let’s you search through previously used commands
Ctrl + C	Kill whatever you are running
Ctrl + D	Exit the current shell
Ctrl + Z	Puts whatever you are running into a suspended background process. fg restores it.
	
Ctrl + W Delete the word before the cursor
Ctrl + K	Clear the line after the cursor
Ctrl + U	Clears the line before the cursor position. If you are at the end of the line, clears the entire line.

Ctrl + Y    Paste after cursor
ALT+ + .      
	
Alt + F	Move cursor forward one word on the current line
Alt + B	Move cursor backward one word on the current line
Tab	Auto-complete files and folder names



nohup app & 
diff /etc/hosts <(ssh somehost cat /etc/hosts)
ssh user@host 'mkdir -p .ssh && cat >> .ssh/authorized_keys' < ~/.ssh/id_rsa.pub
ssh user@server bash < /path/to/local/script.sh
vim scp://username@host//path/to/somefile
env -i app arg1 …  //without environment run app 
```

```
commands
    info thread
    c
end

save breakpoint bp
source bp
```


```
vim -O2 ~/1.xml  ~/1.json

按v：进入面向字符的可视模式；
按V：进入面向行的可视模式；
Ctrl-v：进入面向块的可视模式；

Ctrl-w，s 水平方向分割窗口
Ctrl-w，v 垂直方向分割窗口
Ctrl+w ，q 即q 结束分出来的窗口；
Ctrl+w ，o 即 only! 只显示光标所在窗口，其他窗口隐藏。；

Ctrl+ww
NG
gg
G

*/#
u/ctrl+R
ctrl + I/ctrl + o

shift+k 查找光标所在命令或函数的 man 帮助
• dd 刪除一整行(delete line)， Xdd：删除X行。
• dw 刪除一個单词(delete word)。
• dG 刪至文档结尾。
• d1G 刪至文档头。
• d0 刪至行首。
D 刪至行尾。

:g/George Bush/d删除含××的行
:g!/foo/d  
:6,9w >> /tmp/newfile
:r!<cmd> 将命令 <cmd> 的输出结果插入到当前光标所在位置
:r <file> 将文件 <file> 读入到当前光标所在位置


]c               - advance to the next block with differences
[c               - reverse search for the previous block with differences
do (diff obtain) - bring changes from the other file to the current file
dp (diff put)    - send changes from the current file to the other file
zo               - unfold/unhide text
zc               - refold/rehide text
zr               - unfold both files completely
zm               - fold both files completely
```

