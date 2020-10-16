#include <iostream>
#include <vector>
#include <cstdint>
#include "client_unit_cl.h"
#define DEFAULT_BUFLEN 512
int main()
{
    std::vector<char> charington=std::vector<char>({'P','r','a','z','\0'});
    std::vector<char>::iterator iterc;
    uint8_t var_u8;
    std::vector<char> numeric_vector;
    std::vector<char>::iterator iter;
    std::vector<uint16_t> numeric_vector_16;
    std::vector<uint16_t>::iterator iter_16;
    std::basic_string<char> InBuff;
    uint8_t send_buf[255];
    Client_Unit_cl *ClientUnit = new Client_Unit_cl(
                TCP_Init_Data(AF_INET,SOCK_STREAM,IPPROTO_TCP,
                               DEFAULT_BUFLEN,"64000",
                               "127.0.0.1"));
    std::cout<<"Waiting commands\n";
    ClientUnit->establish_connection();
    while(true)
    {
        std::cin>>InBuff;
        if(InBuff[0]=='$')//it works
        {
            InBuff=InBuff.substr(1,InBuff.size()-1);
            if(InBuff=="sendtest")
            {
                std::cout<<"Send Pack"<<std::endl;
                iterc=charington.begin();
                ClientUnit->send_data(charington.size(),iterc.base());//работает
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
                                int size;
                                iterc=numeric_vector.begin();
                                size=ASN1_solver::code_to_ASN1(static_cast<uint8_t>(*iterc.base()),send_buf);
                                ClientUnit->send_data(size,(char*)send_buf);
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
                size_t pack_size;
                uint8_t buffer[255];
                std::cout<<"Wrote text";
                std::cin>>InBuff;
                pack_size=ASN1_solver::code_to_ASN1(InBuff.begin().base(),
                                          InBuff.size(),buffer);
                ClientUnit->send_data(pack_size,reinterpret_cast<char*>(buffer));
                continue;
            }
            if(InBuff=="connect")
            {
                ClientUnit->establish_connection();
                continue;
            }
            if(InBuff=="exit")
            {
                std::cout<<"Exit"<<std::endl;
                break;
            }
        }
    }
    return 0;
}
