#ifndef ASN1_RESOLVER_H
#define ASN1_RESOLVER_H
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <cstring>
struct ASN1_storage
{

};

class ASN1_resolver
{
    uint8_t get_ASN1_id();
    int get_ASN1_lenght();
    void get_ASN1_Data();
    enum loc_defines {ld_MTU=1500};
    const uint8_t *buff;
    int VAR_class;
    int TAG_ID;

    uint64_t lenght;
    void *data;

    //some useful stuff
    bool is_complex;
    bool is_lengh_indefinite;
    bool is_data_oversized;
public:
    static const int int_sz=sizeof(int);
    ASN1_resolver();
    ASN1_resolver(const uint8_t* str,void *storage);
    ASN1_resolver(std::array<uint8_t,ld_MTU> *arr,ASN1_storage *storage);
    ASN1_resolver(std::vector<uint8_t> &vector,ASN1_storage *storage);
    void resolve(const uint8_t* buff);
    void resolve(const uint8_t* buff, void *dest);
    inline void set_Data_Dst(void *ptr) {data=ptr;}
    inline void* get_Data() {return data;}
};

#endif // ASN1_RESOLVER_H
