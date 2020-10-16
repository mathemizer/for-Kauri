#ifndef SERVER_UNIT_CL_H
#define SERVER_UNIT_CL_H
#include <iostream>
#include <string>
#include "asn1_resolver.h"
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
//#include "server_unit_method_win.h"

class WSA_container
{
  WSADATA wsa;
  bool WSA_state=FALSE;
public:
    WSA_container()
    {
            std::cout<<"Initialising Winsock..."<<"\n";
            if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
            {
                std::cout<<"Failed. Error Code : "<<WSAGetLastError()<<std::endl;
            }
            WSA_state=TRUE;
    }
    bool is_WSA_enable(){return WSA_state;}
    WSADATA* get_Data() {return &wsa;}
};
  #else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
//#include "server_unit_method_linux.h"
#include "my_semaphore.h"
#include <thread>
  #endif

struct TCP_Init_Data
{
    int af;//AF_.../PF_...
    int type;//SOCK_...
    int protocol;//IPPROTO_...(UDP/TCP only(now at least))
    int BufSz;//BUF SZ enough
    const char* NetPort;//0->65535
    const char* NodeName;
    TCP_Init_Data(int Af,
                  int Type,
                  int Protocol,
                  int BufSize,
                  const char* netPort,
                  const char* nodeName);
};
class Server_Unit_cl
{
#ifdef _WIN32
    static WSA_container *WSA_State_Unit;
    SOCKET SU_socket, SU_Listen_Socket;
    sockaddr SU_sockaddr;
    SOCKADDR_IN SU_sockaddrIN;
    WSADATA wsDATA;
    SOCKET ListenSocket;
    SOCKET ClientSocketTCP;
 #else
    int SU_socket, SU_Listen_Socket;
    sockaddr SU_sockaddr;
    sockaddr_in SU_sockaddrIN;
    int ListenSocket;
    int ClientSocketTCP;
 #endif
    int ServerCond;
    char *Input_Buff;
    int Input_Buff_Len=0;
    enum ServerConditions{WSA_INIT=0x1,GET_ADDR=0x2,SOCK_INIT=0x10,
                          BINDED=0x4,LISTENING=0x8,ACCEPTED=0x80,
                          SERVER_CONNECTED=0x100,ECHO_MODE_ON=0x200,
                         SERVER_ONLINE=0x8000};
    wchar_t *ServerName;

    struct sockaddr_in DestAddrTCP;
    addrinfo DestHostInfo,*DestHostInfo_p;
    int State=0;//статус сервера
    struct addrinfo lasthints;
    const char *LastServerName;
    const char* LastNetPort;

    int conn_amt=SOMAXCONN;
public:

    Server_Unit_cl();
    Server_Unit_cl(TCP_Init_Data&& Data);
    ~Server_Unit_cl();
    //Server_Unit_cl();//param
    void set_settings();
    std::basic_string<char>& get_settings();
    int init_socket(TCP_Init_Data&& Data);
    int accept_connection();
    int set_connection();
    int restore_connection();
    int close_connection();
    bool is_wait_for_accept() {if(ServerCond&ACCEPTED) return false;
                              else return true;}
    int close_socket();
    int send_data(unsigned int size, const char *pointer);
    int recive_data();
    int GeneralUIO(); //run into another (from recv() thread) thread
};

#endif // SERVER_UNIT_CL_H
