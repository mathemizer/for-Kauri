#include "client_unit_method_linux.h"
#ifndef _WIN32
Client_Unit_cl::Client_Unit_cl()
{

}
Client_Unit_cl::~Client_Unit_cl()
{
    close_connection();
    closesocket(DstSocketTCP);
    if(Input_Buff)
        delete(Input_Buff);
    std::cout<<"Server destroyed"<<std::endl;
}
Client_Unit_cl::Client_Unit_cl(TCP_Init_Data &&Data)
{
    int iResult;
    struct addrinfo *result=nullptr, *ptr=nullptr, hints;

    SU_Listen_Socket=INVALID_SOCKET;
    if(Data.protocol==IPPROTO_TCP)
    {
        memset(&DestAddrTCP,0,sizeof(DestAddrTCP));

        memset(&hints,0,sizeof(hints));
        hints.ai_family=Data.af;
        hints.ai_socktype=Data.type;
        hints.ai_protocol=Data.protocol;

        iResult=getaddrinfo(Data.NodeName,Data.NetPort,&hints,&result);
        if(iResult!=0)
        {
            std::cout<<"Getaddrinfo failed"<<std::endl;
            return;
        }
        ClientCond|=GET_ADDR;
        DestHostInfo_p=ptr=result;
        DstSocketTCP=socket(ptr->ai_family,
                            ptr->ai_socktype,
                            ptr->ai_protocol);
        if (DstSocketTCP == INVALID_SOCKET)
        {
            std::cout<<"Error at socket():"<<std::endl;
            freeaddrinfo(result);
            return;
        }
        ClientCond|=SOCK_INIT;
    }
    else
    {
        std::cerr<<"Wrong protocol"<<std::endl;
    }
    if(!(Input_Buff=new char[Data.BufSz]))
    {
        std::cout<<"Allocation error server.cpp"<<__LINE__<<std::endl;
        closesocket(DstSocketTCP);
        return;
    }
    Input_Buff_Len=Data.BufSz;
    std::cout<<"Socket ready"<<DstSocketTCP<<std::endl;
    std::cout<<"Client On"<<std::endl;
}
//Client_Unit_cl();//param
void Client_Unit_cl::set_settings()
{

}
std::basic_string<char>& Client_Unit_cl::get_settings()
{
    static std::basic_string<char> Input_Settings_Buff;

    return Input_Settings_Buff;
}
int Client_Unit_cl::init_socket(TCP_Init_Data &&Data)
{
    return 0;
}

int Client_Unit_cl::establish_connection()
{
    if(connect(DstSocketTCP,DestHostInfo_p->ai_addr,DestHostInfo_p->ai_addrlen)!=0)
    {
        std::cout<<"Connection failed"<<std::endl;
        ClientCond&=~CONNECTED;
        State=0;
        return 1;
    }
    std::cout<<"Connection accepted\n";
    ClientCond|=CONNECTED;
    State=CLIENT_ONLINE;
    return 0;
}

int Client_Unit_cl::set_connection()
{
    return 0;
}
int Client_Unit_cl::restore_connection()
{
    return 0;
}
int Client_Unit_cl::close_connection()
{
    int iResult;
    iResult=shutdown(DstSocketTCP,SD_SEND);
    if(iResult==SOCKET_ERROR)
    {
        std::cout<<"Shutdown failed"<<std::endl;
        closesocket(DstSocketTCP);
        ClientCond=State=0;
        return 1;
    }
    return 0;
}
int Client_Unit_cl::close_socket()
{
    return 0;
}
int Client_Unit_cl::send_data(size_t size, const char *pointer)
{
    int iSendResult;
    iSendResult=send(DstSocketTCP,pointer,size,0);
    if(iSendResult==SOCKET_ERROR)
        {
        std::cout<<"Send failed"<<std::endl;
        closesocket(DstSocketTCP);
        ClientCond=State=0;
        return 1;
        }
    return 0;
}
int Client_Unit_cl::recive_data()
{
    int iRecvResult;
    iRecvResult=recv(DstSocketTCP,Input_Buff,Input_Buff_Len,NULL);

    if(iRecvResult>0)//сообщение принято
    {

    }
    else if(iRecvResult==0)//коннект закрыт
    {
        std::cout<<"Connect closed\n";
        closesocket(DstSocketTCP);
        ClientCond&=~CONNECTED;
        State=0;
    }
    else//iResult<0 - ошибка приёма
    {
    std::cout<<"Recive failed: "<<" "<<__FILE__<<" "<<__LINE__<<"\n";
    closesocket(DstSocketTCP);
    ClientCond=State=0;
    return 1;
    }
    return 0;
}

Server_Unit_method_win::Server_Unit_method_win()
{

}

#endif
