#include <iostream>
#include <vector>

#include "server_unit_cl.h"

#define DEFAULT_BUFLEN 512

#ifndef _WIN32
semaphore *Smphr;
int GUIO_Thread(void *FMPtr);
#else 
DWORD WINAPI GUIO_Thread(LPVOID FMPtr);
#endif

int main()
{
#ifdef _WIN32
    DWORD GUIOThreadID;
    LPVOID GUIOThreadVariable;
    HANDLE GUIOThreadhWnd;
    #else
    #endif
    char c[128];
    std::cout << "Hello World!" << std::endl;
    Server_Unit_cl *ServerUnit=new Server_Unit_cl(
        TCP_Init_Data(AF_INET,SOCK_STREAM,IPPROTO_TCP,
                      DEFAULT_BUFLEN,"64000",
                      "127.0.0.1"));
    #ifdef _WIN32
    GUIOThreadVariable=ServerUnit;
    HANDLE GUIOSemaphore;

    GUIOSemaphore=CreateSemaphoreW(NULL,1,1,L"GUIO");
    GUIOThreadhWnd=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)GUIO_Thread,GUIOThreadVariable,0,&GUIOThreadID);
    #else
    int GUIOThreadID;
    void *GUIOThreadVariable;
    unsigned int GUIOThreadhWnd;
    Smphr=new semaphore("GUIO");
    std::thread GUIO_Thread_Obj(GUIO_Thread,GUIOThreadVariable);
    #endif
    if(ServerUnit!=nullptr)
        while(1)
        {
            while (!ServerUnit->is_wait_for_accept())
            {
                ServerUnit->recive_data();
            }
            std::cout<<"Connection closed. Try to accept? (Y/N)";
            #ifdef _WIN32
            WaitForSingleObject(GUIOSemaphore,INFINITE);
            #else
            Smphr.Passing();
            #endif
            ServerUnit->accept_connection();
        }
    else
        std::cout<<"main.c memory allocation failure"<<std::endl;
    return 0;
}
#ifdef _WIN32
DWORD WINAPI GUIO_Thread(LPVOID FMPtr)
{
#else
int GUIO_Thread(void *FMPtr)
{
#endif
    std::vector<char> charington=std::vector<char>({'P','r','a','z','\0'});
    std::vector<char>::iterator iterc;
    uint8_t var_u8;
    std::vector<char> numeric_vector;
    std::vector<char>::iterator iter;
    std::vector<uint16_t> numeric_vector_16;
    std::vector<uint16_t>::iterator iter_16;
    std::basic_string<char> InBuff;
    Server_Unit_cl *ServerUnit=static_cast<Server_Unit_cl*>(FMPtr);
    #ifdef _WIN32
    HANDLE GUIOSemaphor;
    while(!(GUIOSemaphor=OpenSemaphoreW(SEMAPHORE_MODIFY_STATE,FALSE,L"GUIO")));
    #else
    #endif
    std::cout<<"Waiting commands\n";
    while(true)
    {
        if(ServerUnit==nullptr)
            break;
        std::cin>>InBuff;
        if(InBuff[0]=='$')//it works
        {
            if(InBuff=="sendtest")
            {
                std::cout<<"Send Pack"<<std::endl;
                iterc=charington.begin();//
                ServerUnit->send_data(charington.size(),iterc.base());//работает
                continue;
            }
            if(InBuff=="send_num")
            {
                while(true)
                {
                    std::cin>>InBuff;
                    if(InBuff.find_first_not_of("0123456789")!=std::basic_string<char>::npos)
                    {
                        if(InBuff=="send")
                        {
                            std::cout<<"Sending Pack. Pack size: "<<numeric_vector.size()<<std::endl;
                            if(numeric_vector.size())
                            {
                                iterc=numeric_vector.begin();
                                ServerUnit->send_data(numeric_vector.size(),iterc.base());
                                std::cout<<"Send Pack. Pack size: "<<numeric_vector.size()<<std::endl;
                                numeric_vector.clear();
                            }
                            else
                            {
                                std::cout<<"Pack is empty. Sending cancelled."<<std::endl;
                            }
                            break;
                        }
                        else
                        if(InBuff=="hex")
                        {
                            std::cout<<"Switch to hex"<<std::endl;
                        }
                        else if(InBuff=="cancel")
                        {
                            std::cout<<"Wrong command\n";
                            break;
                        }
                        else
                        std::cout<<"Wrong command"<<std::endl;
                    }
                    else
                    {
                        numeric_vector.push_back(var_u8=std::stoi(InBuff));
                        std::cout<<"Stacked "<<static_cast<int>(var_u8)<<std::endl;
                    }
                }
                continue;
            }
            if(InBuff=="send_text")
            {
                std::cout<<"Wrote text";
                std::cin>>InBuff;
                ServerUnit->send_data(InBuff.size(),InBuff.c_str());
                continue;
            }

            if(InBuff=="accept")
            {
                char str[5];
                if(ServerUnit->is_wait_for_accept())
                {
                    #ifdef _WIN32
                    ReleaseSemaphore(GUIOSemaphor,1,nullptr);
                    #else
                    Smphr.Release();
                    #endif
                }
                else
                {
                    std::cout<<"Already accepted\n";
                }
                std::cout<<"Connection closed. Try to accept? (Y/N)";
                std::cin>>str;
                if((str[0]=='n')||(str[0]=='N'))
                    break;
                continue;
            }
            if(InBuff=="exit")
            {
                delete ServerUnit;
                ServerUnit=nullptr;
                std::cout<<"Exit"<<std::endl;
                break;
            }
        }
        if((InBuff[0]=='y') || (InBuff[0]=='Y'))
        {
            if(ServerUnit->is_wait_for_accept())
                #ifdef _WIN32
                ReleaseSemaphore(GUIOSemaphor,1,nullptr);
                #else
                Smphr.Release();
                #endif
        }
        if((InBuff[0]=='n') || (InBuff[0]=='N'))
        {
            delete ServerUnit;
            ServerUnit=nullptr;
        }
    }
    return 0;
}
