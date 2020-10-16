#include "client_unit_method_win.h"
#ifdef _WIN32
WSA_container* Client_Unit_cl::WSA_State_Unit=nullptr;
Client_Unit_cl::Client_Unit_cl()
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
Client_Unit_cl::~Client_Unit_cl()
{
    close_connection();
    closesocket(DstSocketTCP);
    if(!ClientCond)
        delete(WSA_State_Unit);
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
        ClientCond|=WSA_INIT;

        ZeroMemory(&hints,sizeof(hints));
        hints.ai_family=Data.af;
        hints.ai_socktype=Data.type;
        hints.ai_protocol=Data.protocol;

        iResult=getaddrinfo(Data.NodeName,Data.NetPort,&hints,&result);
        if(iResult!=0)
        {
            std::cout<<"Getaddrinfo failed"<<std::endl;
            //WSACleanup();
            //надо сделать связку через класс WSA_State_Unit
            return;
        }
        ClientCond|=GET_ADDR;
        DestHostInfo_p=ptr=result;
        DstSocketTCP=socket(ptr->ai_family,
                            ptr->ai_socktype,
                            ptr->ai_protocol);
        if (DstSocketTCP == INVALID_SOCKET)
        {
            std::cout<<"Error at socket():"<<WSAGetLastError()<<std::endl;
            freeaddrinfo(result);
            WSACleanup();
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
        WSACleanup();
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
        std::cout<<"Connection failed"<<
                WSAGetLastError()<<std::endl;
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
        std::cout<<"Shutdown failed"<<WSAGetLastError()<<std::endl;
        closesocket(DstSocketTCP);
        WSACleanup();
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
        std::cout<<"Send failed"<<WSAGetLastError()<<std::endl;
        closesocket(DstSocketTCP);
        WSACleanup();
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
    std::cout<<"Recive failed: "<<WSAGetLastError()<<" "<<__FILE__<<" "<<__LINE__<<"\n";
    closesocket(DstSocketTCP);
    WSACleanup();
    ClientCond=State=0;
    return 1;
    }
    return 0;
}

Server_Unit_method_win::Server_Unit_method_win()
{

}
#endif
