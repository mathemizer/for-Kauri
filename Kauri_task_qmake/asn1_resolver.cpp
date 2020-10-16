#include "asn1_resolver.h"

uint8_t ASN1_resolver::get_ASN1_id()
{
    is_complex=false;
    VAR_class=(*buff&0xC0);
    if(*buff&0x20)
        is_complex=true;
    if((*buff&0x1F)<0x1F)
    {
        buff++;
        do
        {
            TAG_ID+=*buff&0x7F;
        }while(*buff++&0x80); //check bit7
    }
    else //simple ID
    {
        TAG_ID=*buff++&0x1F;
    }
    return 0;
}

int ASN1_resolver::get_ASN1_lenght()
{
    is_lengh_indefinite=false;
    if(*buff&0x80) //extended defined or indefinite forms
    {
        uint8_t i=*buff&0x7F;
        if(i)  //defined form
        {
            if(*buff++!=0xFF)
            {
                std::cout<<"Wrong 1'st octet of defined form lenght\n";
                return -1;
            }
            if(i>8)
            {
                std::cout<<"Standart said that is ok, but data size more than 2^64.\n\
                                Too long frame;\n";
                is_data_oversized=true;
                return -2;
            }
            else
            {
                memcpy(&lenght,buff,i);
                buff+=i;
            }
        }
        else            //undefined form
        {
            is_lengh_indefinite=true;
        }
    }
    else //short forms
    {
        lenght=*buff++;
    }
    std::cout<<"Lenght = "<<lenght<<"\n";
    //in result buff move to the /void *data/ adress
    return 0;
}

void ASN1_resolver::get_ASN1_Data()
{
    if(is_lengh_indefinite==false)
    {
    if(data!=nullptr)
    memcpy(data,buff,lenght);
    else
        std::cout<<"data uninitialised!\n"<<std::endl;
    }
    else
    {

    }
}
ASN1_resolver::ASN1_resolver()
{
    buff=nullptr;
    data=nullptr;
    is_complex=false;
    is_lengh_indefinite=false;
    is_data_oversized=false;
}

ASN1_resolver::ASN1_resolver(const uint8_t *str, void *storage)
{

}

ASN1_resolver::ASN1_resolver(std::array<uint8_t, ASN1_resolver::ld_MTU> *arr, ASN1_storage *storage)
{

}

ASN1_resolver::ASN1_resolver(std::vector<uint8_t> &vector, ASN1_storage *storage)
{

}
void ASN1_resolver::resolve(const uint8_t *buff)
{
    if((*buff&0xC0)==0)
    {
        if(*buff&0x20)
        {
            std::cout<<"Complex type code\n"<<std::endl;
            return;
        }
        std::cout<<"Universal class Message:\nTag Type: ";
        if((*buff++&0x1F)<31)
        {
            std::cout<<"Short   Lenght type: ";
            if(*buff&0x80)
            {
                uint8_t data_lenght_sz, i_cnt;
                uint64_t data_lenght=0;
                std::cout<<"Long    Size:";
                if(*buff==0xFF)
                {
                    std::cout<<"Size error"<<std::endl;
                    return;
                }
                if((data_lenght_sz=*buff++&0x7F)<9)
                for(i_cnt=0;i_cnt<data_lenght_sz;i_cnt++)
                    data_lenght|=((uint64_t)buff[i_cnt])<<(i_cnt<<3);
                else
                {
                    std::cout<<"Lenght error"<<std::endl;
                    return;
                }
                std::cout<<data_lenght;
            }
            else
            {
                std::cout<<"Short   Size: "<<static_cast<size_t>(*buff)<<"\n";
            }
        }
        else
        {
            std::cout<<"Long ";
            std::cout<<"Skipped ";
            return;
        }
    }
    else
    {
        std::cout<<"Non universal class\n";
    }
}

void ASN1_resolver::resolve(const uint8_t *buff, void *dest)
{

}
