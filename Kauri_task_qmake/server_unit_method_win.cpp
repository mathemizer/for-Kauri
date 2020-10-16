#include "server_unit_method_win.h"
#ifdef _WIN32
WSA_container* Server_Unit_cl::WSA_State_Unit=nullptr;
Server_Unit_cl::Server_Unit_cl()
{
    if(WSA_State_Unit==nullptr)
        if((WSA_State_Unit = new WSA_container())==nullptr)
            {
                std::cout<<"memory Allocation failure"<<std::endl;
                exit(1);
            }
    if(WSA_State_Unit->is_WSA_enable()==false)
    {
        if (WSAStartup(MAKEWORD(2,2),WSA_State_Unit->get_Data()) != 0)
        {
            std::cout<<"Failed. Error Code : "<<WSAGetLastError()<<std::endl;
            exit(1);
        }
    }
}
Server_Unit_cl::~Server_Unit_cl()
{
    close_connection();
    closesocket(ListenSocket);
    closesocket(ClientSocketTCP);
    if(!ServerCond)
        delete(WSA_State_Unit);
    if(Input_Buff)
        delete(Input_Buff);
    std::cout<<"Server destroyed"<<std::endl;
}
Server_Unit_cl::Server_Unit_cl(TCP_Init_Data &&Data)
{
    int iResult;
    int loccnt;
    struct addrinfo *result=NULL, *ptr=NULL, hints;

    SU_Listen_Socket=INVALID_SOCKET;
    if(Data.protocol==IPPROTO_TCP)
    {
        if(WSA_State_Unit==nullptr)
            if((WSA_State_Unit = new WSA_container())==nullptr)
                {
                    std::cout<<"memory Allocation failure"<<std::endl;
                    exit(1);
                }
        ZeroMemory(&DestAddrTCP,sizeof(DestAddrTCP));
        // Check Winsock
        if(WSA_State_Unit->is_WSA_enable()==false)
        {
            if (WSAStartup(MAKEWORD(2,2),WSA_State_Unit->get_Data()) != 0)
            {
                std::cout<<"Failed. Error Code : "<<WSAGetLastError()<<std::endl;
                exit(1);
            }
        }
        ServerCond|=WSA_INIT;
        ZeroMemory(&hints,sizeof(hints));
        hints.ai_family=Data.af;
        hints.ai_socktype=Data.type;
        hints.ai_protocol=Data.protocol;
        hints.ai_flags=AI_PASSIVE;

        iResult=getaddrinfo(Data.NodeName,Data.NetPort,&hints,&result);
        if(iResult!=0)
        {
            std::cout<<"Getaddrinfo failed"<<std::endl;
            //WSACleanup();
            //надо сделать связку через класс WSA_State_Unit
            return;
        }
        ServerCond|=GET_ADDR;
        lasthints=hints;
        LastServerName=Data.NodeName;
        LastNetPort=Data.NetPort;
        ptr=result;
        SU_Listen_Socket=socket(ptr->ai_family,
                            ptr->ai_socktype,
                            ptr->ai_protocol);
        if (SU_Listen_Socket == INVALID_SOCKET)
        {
            std::cout<<"Error at socket():"<<WSAGetLastError()<<std::endl;
            freeaddrinfo(result);
            WSACleanup();
            return;
        }
        ServerCond|=SOCK_INIT;
        //биндим сокет
        iResult = bind( SU_Listen_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            std::cout<<"bind failed with error: "<<
                       WSAGetLastError()<<std::endl;
            freeaddrinfo(ptr);
            closesocket(SU_Listen_Socket);
            WSACleanup();
            return;
        }
        ServerCond|=BINDED;
        // начинаем слушать сокет, замечу что это как бы
        //аналог функции Read, т.е. не потоковый
        if ( listen( SU_Listen_Socket, conn_amt ) == SOCKET_ERROR )
        {
            std::cout<<"Listen failed with error: "<<
                       WSAGetLastError()<<std::endl;
            closesocket(SU_Listen_Socket);
            WSACleanup();
            return;
        }
        ServerCond|=LISTENING;
    }
    else
    {
        std::cerr<<"Wrong protocol"<<std::endl;
    }
    if(!(Input_Buff=new char[Data.BufSz]))
    {
        std::cout<<"Allocation error server.cpp"<<__LINE__<<std::endl;
        closesocket(SU_Listen_Socket);
        WSACleanup();
        return;
    }

    Input_Buff_Len=Data.BufSz;

    State=SERVER_ONLINE;
    std::cout<<"Listen socket"<<SU_Listen_Socket<<std::endl;
    std::cout<<"Server On"<<std::endl;
}
//Server_Unit_cl();//param
void Server_Unit_cl::set_settings()
{

}
std::basic_string<char>& Server_Unit_cl::get_settings()
{
    static std::basic_string<char> Input_Settings_Buff;

    return Input_Settings_Buff;
}
int Server_Unit_cl::init_socket(TCP_Init_Data &&Data)
{
    return 0;
}

int Server_Unit_cl::accept_connection()
{
    ClientSocketTCP=accept(SU_Listen_Socket,NULL,NULL);
    if(ClientSocketTCP==INVALID_SOCKET)
    {
        std::cout<<"Accept failed"<<
                WSAGetLastError()<<std::endl;
        closesocket(SU_Listen_Socket);
        WSACleanup();
        ServerCond=0;
        State=0;
        return 1;
    }
    std::cout<<"Connection accepted\n";
    ServerCond|=ACCEPTED;
    State=SERVER_ONLINE;
    return 0;
}
int Server_Unit_cl::set_connection()
{
    return 0;
}
int Server_Unit_cl::restore_connection()
{
    return 0;
}
int Server_Unit_cl::close_connection()
{
    int iResult;
    iResult=shutdown(ClientSocketTCP,SD_SEND);
    if(iResult==SOCKET_ERROR)
    {
        std::cout<<"Shutdown failed"<<WSAGetLastError()<<std::endl;
        closesocket(ClientSocketTCP);
        WSACleanup();
        ServerCond=State=0;
        return 1;
    }
    return 0;
}
int Server_Unit_cl::close_socket()
{
    return 0;
}
int Server_Unit_cl::send_data(unsigned int size,const char *pointer)
{
    unsigned int iSendResult;
    iSendResult=send(ClientSocketTCP,pointer,size,0);//echo send
    if(iSendResult==SOCKET_ERROR)
        {
        std::cout<<"Send failed"<<WSAGetLastError()<<std::endl;
        closesocket(ClientSocketTCP);
        WSACleanup();
        ServerCond=State=0;
        return 1;
        }
    return 0;
}
int Server_Unit_cl::recive_data()
{
    int iRecvResult;
    iRecvResult=recv(ClientSocketTCP,Input_Buff,Input_Buff_Len,NULL);

    if(iRecvResult>0)//сообщение принято
    {
        ASN1_resolver().resolve(reinterpret_cast<uint8_t*>(Input_Buff));
    }
    else if(iRecvResult==0)//коннект закрыт
    {
        std::cout<<"Connect closed\n";
        closesocket(ClientSocketTCP);
        ServerCond&=~ACCEPTED;
        State=0;
    }
    else//iResult<0 - ошибка приёма
    {
    std::cout<<"Recive failed: "<<WSAGetLastError()<<" "<<__FILE__<<" "<<__LINE__<<"\n";
    closesocket(ListenSocket);
    closesocket(ClientSocketTCP);
    WSACleanup();
    ServerCond=State=0;
    return 1;
    }
    return 0;
}

Server_Unit_method_win::Server_Unit_method_win()
{

}
#endif
