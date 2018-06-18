Outline
- process model
- thread model
  - 在Chrome下你可以这样来做：生成一个Task，放到B线程的队列中，在该Task的Run方法最后，会生成另一个Task，这个Task会放回到A的线程队列，由A来执行。
  - channel/syncchannel/channelproxy
  
  
  https://blog.csdn.net/michellechouu/article/details/46521759
  
  
  unix AF_LOCAL can be used to pass descriptor
