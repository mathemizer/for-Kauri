#include "asn1_solver.h"

uint8_t ASN1_solver::get_ASN1_id()
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

int ASN1_solver::get_ASN1_lenght()
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
                buff+=lenght;
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
    //in result buff move to the /void *data/ adress
    return 0;
}

void ASN1_solver::get_ASN1_Data()
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
ASN1_solver::ASN1_solver()
{
    buff=nullptr;
    data=nullptr;
    is_complex=false;
    is_lengh_indefinite=false;
    is_data_oversized=false;
}
/*
ASN1_solver::ASN1_solver(const uint8_t *str, size_t size, uint8_t *storage)
{
    *storage++=2; //номер Тэга типа Integer
    if(size<128)
    *storage++=size; //size of data
    else
    {
        *storage=(1<<7);
        if(size<~(uint16_t)0)
        {
            *storage++|=0x2;
            *reinterpret_cast<uint16_t*>(storage)=size;
            storage+=sizeof(uint16_t);
        }
        else if(size<~(uint32_t)0)
        {
            *storage++|=0x4;
            *reinterpret_cast<uint32_t*>(storage)=size;
            storage+=sizeof(uint32_t);
        }
        else if(size<~(uint64_t)0)
        {
            *storage++|=0x8;
            *reinterpret_cast<uint64_t*>(storage)=size;
            storage+=sizeof (uint64_t);
        }
        else
        {
            std::cout<<"Too long string\n";
            return;
        }
    }
    lenght=size;
    memcpy(storage,str,size);
}*/
ASN1_solver::ASN1_solver(const int8_t *str, size_t size, uint8_t *storage)
{
    *storage++=19; //номер Тэга типа Printable String
    if(size<128)
    *storage++=size; //size of data
    else
    {
        *storage=(1<<7);
        if(size<~(uint16_t)0)
        {
            *storage++|=0x2;
            *reinterpret_cast<uint16_t*>(storage)=size;
            storage+=sizeof(uint16_t);
        }
        else if(size<~(uint32_t)0)
        {
            *storage++|=0x4;
            *reinterpret_cast<uint32_t*>(storage)=size;
            storage+=sizeof(uint32_t);
        }
        else if(size<~(uint64_t)0)
        {
            *storage++|=0x8;
            *reinterpret_cast<uint64_t*>(storage)=size;
            storage+=sizeof (uint64_t);
        }
        else
        {
            std::cout<<"Too long string\n";
            return;
        }
    }
    lenght=size;
    memcpy(storage,str,size);
}
ASN1_solver::ASN1_solver(std::array<uint8_t, ASN1_solver::ld_MTU> *arr, ASN1_storage *storage)
{

}

ASN1_solver::ASN1_solver(std::vector<uint8_t> &vector, ASN1_storage *storage)
{

}

size_t ASN1_solver::code_to_ASN1(const char *ptr, size_t size, uint8_t *buffer)
{
    size_t head_size=0;
    *buffer++=2; //номер Тэга типа Integer
    if(size<128)
    {
        *buffer++=size; //size of data
        head_size+=2;
    }
    else
    {
        *buffer=(1<<7);
        if(size<=~(uint8_t)0)
        {
            *buffer++|=0x1;
            *reinterpret_cast<uint16_t*>(buffer)=size;
            buffer+=sizeof(uint16_t);
            head_size+=3;
        }
        else if(size<=~(uint16_t)0)
        {
            *buffer++|=0x2;
            *reinterpret_cast<uint16_t*>(buffer)=size;
            buffer+=sizeof(uint16_t);
            head_size+=4;
        }
        else if(size<=~(uint32_t)0)
        {
            *buffer++|=0x4;
            *reinterpret_cast<uint32_t*>(buffer)=size;
            buffer+=sizeof(uint32_t);
            head_size+=6;
        }
        else if(size<=~(uint64_t)0)
        {
            *buffer++|=0x8;
            *reinterpret_cast<uint64_t*>(buffer)=size;
            buffer+=sizeof (uint64_t);
            head_size+=10;
        }
        else
        {
            std::cout<<"Too long string\n";
            return -1;
        }
    }
    memcpy(buffer,ptr,size);
    return size+head_size;
}


int ASN1_solver::code_to_ASN1(uint8_t integer, uint8_t *buffer)
{
    *buffer++=2; //номер Тэга типа Integer
    *buffer++=1; //lenght of data
    *buffer=integer;
    return 1+1+1;
}
int ASN1_solver::code_to_ASN1(uint16_t integer, uint8_t *buffer)
{
    *buffer++=2; //номер Тэга типа Integer
    *buffer++=2; //lenght of data
    *reinterpret_cast<uint16_t*>(buffer)=integer;
    return 1+1+2;
}
int ASN1_solver::code_to_ASN1(uint32_t integer, uint8_t *buffer)
{
    *buffer++=2; //номер Тэга типа Integer
    *buffer++=4; //lenght of data
    *reinterpret_cast<uint32_t*>(buffer)=integer;
    return 1+1+4;
}
int ASN1_solver::code_to_ASN1(uint64_t integer, uint8_t *buffer)
{
    *buffer++=2; //номер Тэга типа Integer
    *buffer++=8; //lenght of data
    *reinterpret_cast<uint64_t*>(buffer)=integer;
    return 1+1+8;
}
void ASN1_solver::code_to_ASN1(uint8_t integer, std::vector<uint8_t> *buffer)
{
    buffer->push_back(2); //номер Тэга типа Integer
    buffer->push_back(1); //lenght of data
    buffer->push_back(integer);
}
void ASN1_solver::code_to_ASN1(uint16_t integer, std::vector<uint8_t> *buffer)
{
    buffer->push_back(2); //номер Тэга типа Integer
    buffer->push_back(2); //lenght of data
    buffer->push_back(static_cast<uint8_t>(integer));
    buffer->push_back(static_cast<uint8_t>(integer>>8));
}
void ASN1_solver::code_to_ASN1(uint32_t integer, std::vector<uint8_t> *buffer)
{
    int i_cnt;
    buffer->push_back(2); //номер Тэга типа Integer
    buffer->push_back(4); //lenght of data
    for(i_cnt=0;i_cnt<32;i_cnt+=8)
    buffer->push_back(static_cast<uint8_t>(integer>>i_cnt));
}
void ASN1_solver::code_to_ASN1(uint64_t integer, std::vector<uint8_t> *buffer)
{
    int i_cnt;
    buffer->push_back(2); //номер Тэга типа Integer
    buffer->push_back(8); //lenght of data
    for(i_cnt=0;i_cnt<64;i_cnt+=8)
    buffer->push_back(static_cast<uint8_t>(integer>>i_cnt));
}
void ASN1_solver::resolve(const uint8_t *buff)
{

}

void ASN1_solver::resolve(const uint8_t *buff, void *dest)
{

}
