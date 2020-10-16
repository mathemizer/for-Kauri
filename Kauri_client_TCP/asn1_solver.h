#ifndef ASN1_SOLVER_H
#define ASN1_SOLVER_H
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <cstring>
struct ASN1_storage
{

};

class ASN1_solver
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
    ASN1_solver();
    //ASN1_solver(const uint8_t *str, size_t size, uint8_t *storage);
    ASN1_solver(const int8_t *str, size_t size, uint8_t *storage);
    ASN1_solver(std::array<uint8_t,ld_MTU> *arr,ASN1_storage *storage);
    ASN1_solver(std::vector<uint8_t> &vector,ASN1_storage *storage);
    static size_t code_to_ASN1(const char *ptr, size_t size, uint8_t *buffer);
    std::vector<int8_t> &code_to_ASN1(std::basic_string<char> , size_t size, uint8_t *buffer);

    static int code_to_ASN1(uint8_t integer, uint8_t *buffer);
    static int code_to_ASN1(uint16_t integer, uint8_t *buffer);
    static int code_to_ASN1(uint32_t integer, uint8_t *buffer);
    static int code_to_ASN1(uint64_t integer, uint8_t *buffer);
    static void code_to_ASN1(uint8_t integer, std::vector<uint8_t> *buffer);
    static void code_to_ASN1(uint16_t integer, std::vector<uint8_t> *buffer);
    static void code_to_ASN1(uint32_t integer, std::vector<uint8_t> *buffer);
    static void code_to_ASN1(uint64_t integer, std::vector<uint8_t> *buffer);

    static void resolve(const uint8_t* buff);
    static void resolve(const uint8_t* buff, void *dest);
    inline void set_Data_Dst(void *ptr) {data=ptr;}
    inline void* get_Data() {return data;}
};

#endif // ASN1_SOLVER_H
