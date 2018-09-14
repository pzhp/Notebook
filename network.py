import threading
import socket
import sys
import os
import datetime


mode = sys.argv[1]
host = sys.argv[2]
port = int(sys.argv[3])
sfile = sys.argv[4]


def client():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		
	s.connect((host, port))
	
	data = None
	try:
		f = open(sfile)
		data = f.read()
	except:
		print "failed to read file"
		return
	
	print "start send data ...{}".format(len(data))
	st = datetime.datetime.now()
	s.send(data)
	print "send time: ", datetime.datetime.now() - st
	resp = s.recv(1024)	
        print "replay {}".format(resp)
	print "end time: ",  datetime.datetime.now() - st	
	
def handler(sock, addr):
	print 'Accept new connection from {}...'.format(addr)
	total = 14169936
	sum = 0
	while True:
		data = sock.recv(1024*1024)
		sum = sum + len(data)
		if sum >= total:
			break
		print "receiving data {}".format(len(data))
		if not data:
			break
	sock.send("done ok !!!!")
	sock.close()
	print "Connection from {}:{} is closed".format(sock, addr)
		

def server():
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind(('0.0.0.0', port))
	s.listen(5)
	print 'Waiting for connection...'
	
	while True:
		sock, addr = s.accept()
		print "accept one"
		t = threading.Thread(target= handler, args=(sock, addr))
		t.start()

if mode == 'client':
	client()
elif mode == 'server':
	server()
else:
	print "unrecogize cmd"	
