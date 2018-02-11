#include "tcpclient.h"

namespace ctrler
{

  TCPClient::TCPClient()
  {
    _sock = -1;
    _port = 0;
    _address = "";
  }

  bool TCPClient::Setup(std::string& _address , int _port)
  {
    if(_sock == -1)
      {
	_sock = socket(AF_INET , SOCK_STREAM , 0);
	if (_sock == -1)
	  std::cout << "Could not create socket" << std::endl;
      }
    if(inet_addr(_address.c_str()) == -1)
      {
	struct hostent *he;
	struct in_addr **addr_list;
	if ((he = gethostbyname(_address.c_str())) == NULL)
	  {
	    herror("gethostbyname");
	    std::cout<<"Failed to resolve hostname\n";
	    return false;
	  }
	addr_list = (struct in_addr **) he->h_addr_list;
	for(int i = 0; addr_list[i] != NULL; i++)
	  {
	    _server.sin_addr = *addr_list[i];
	    break;
	  }
      }
    else
      _server.sin_addr.s_addr = inet_addr(_address.c_str());
  
    _server.sin_family = AF_INET;
    _server.sin_port = htons(_port);
    if (connect(_sock, (struct sockaddr*)&_server , sizeof(_server)) < 0)
      {
	perror("connect failed. Error");
	return false;
      }
    return true;
  }

  bool TCPClient::Send(std::string& data)
  {
    if(_sock != -1)
      {
	if(send(_sock, data.c_str(), strlen(data.c_str()), 0) < 0)
	  {
	    std::cout << "Send failed : " << data << std::endl;
	    return false;
	  }
      }
    else
      return false;
    return true;
  }

  void TCPClient::Receive(std::string& data, int size)
  {
    char buffer[size];
    memset(&buffer[0], 0, sizeof(buffer));

    if(recv(_sock, buffer, size, 0) < 0)
      {
	std::cout << "receive failed!" << std::endl;
	data = "";
      }
    buffer[size-1]='\0';
    data = buffer;
  }

  std::string TCPClient::Read()
  {
    char buffer[1] = {};
    std::string reply;
    while(buffer[0] != '\n')
      {
	if(recv(_sock, buffer, sizeof(buffer), 0) < 0)
	  {
	    std::cout << "receive failed!" << std::endl;
	    return nullptr;
	  }
	reply += buffer[0];
      }
    return reply;
  }

  void TCPClient::Exit()
  {
    close(_sock);
  }

}
