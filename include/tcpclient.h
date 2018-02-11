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

namespace ctrler
{
  class TCPClient
  {
  public:
    /**
       Constructor
     */
    TCPClient();
    
    /**
       Set up TCP socket client
       @params
       address: server ip address
       port: server port number
       @return
       a bool value indicating whether the connection is established or not
     */
    bool Setup(std::string& address, int port);

    /**
       Send data
       @params
       data: data string
       @return
       a bool value indicating whether the data is sent or not
     */
    bool Send(std::string& data);

    /**
       Receive data
       @params
       data: received data buffer
       size: expected size
       @return
     */
    void Receive(std::string& data, int size = 4096);

    /**
       Read individual character as a 1-element string
       @params
       @return
       Received character
     */
    std::string Read();

    /**
       Close socket. Terminate client
     */
    void Exit();

  private:
    int _sock;
    std::string _address;
    int _port;
    struct sockaddr_in _server;

  };
}

#endif
