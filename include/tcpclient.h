#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h> 
#include <vector>

class TCPClient
{
 private:
  int sock;
  std::string address;
  int port;
  struct sockaddr_in server;

 public:
  TCPClient();
  bool Setup(std::string& address, int port);
  bool Send(std::string& data);
  std::string Receive(int size = 4096);
  std::string Read();
  void Exit();
};

#endif
