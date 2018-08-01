Top commands

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
