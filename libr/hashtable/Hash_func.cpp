#include "Hash_table.hpp"
#include <string.h>
#include <functional>
#include <x86intrin.h>
#include "../../Compiler/compiler.h"

#define MY_STRLEN(key) (key == nullptr) ? 0 : strlen(key)

template <>
List_pointer_t<char*, size_t>* List_pointer<char*, size_t> :: find_value(char* key)
{
    assert(this != nullptr);

    List_pointer_t<char*, size_t>* current_element = head();

    for (size_t index = 1; index <= size_; ++index)
    {
        if (!strcmp(current_element->key, key))
        {
            return current_element;
        }

        current_element = next(current_element);
    }

    return nullptr;
}

template <>
List_pointer_t<char*, struct Variable>* List_pointer<char*, struct Variable> :: find_value(char* key)
{
    assert(this != nullptr);

    List_pointer_t<char*, struct Variable>* current_element = head();

    for (size_t index = 1; index <= size_; ++index)
    {
        if (!strcmp(current_element->key, key))
        {
            return current_element;
        }

        current_element = next(current_element);
    }

    return nullptr;
}

template <>
void Hash_table<char*, struct Variable> :: visitor()
{
    size_t number_list = 0;

    while (number_list < get_size())
    {
        List_pointer_t<char*, struct Variable>* current_node = keys_[number_list].head();

        while (current_node != nullptr)
        {
            current_node->value.initialization = 1;

            current_node = current_node->next;
        }
         
        ++number_list;
    } 
}


unsigned int hash_crc(char* key)
{
    unsigned int hash = 0;
    
    size_t len = MY_STRLEN(key);
    const unsigned int* data = (const unsigned int*)key;

    while(len >= 4)
    {
        hash = _mm_crc32_u32(hash, *data);

        ++data;
        len -= 4;
    }

    switch (len)
    {
        case 1:
            hash = _mm_crc32_u8(hash, ((const unsigned char*)(data))[0]);
            break;
        
        case 2:           
            hash = _mm_crc32_u8(hash, ((const unsigned char*)(data))[0]);
            hash = _mm_crc32_u8(hash, ((const unsigned char*)(data))[1]);
            break;

        case 3:
            hash = _mm_crc32_u8(hash, ((const unsigned char*)(data))[0]);
            hash = _mm_crc32_u8(hash, ((const unsigned char*)(data))[1]);
            hash = _mm_crc32_u8(hash, ((const unsigned char*)(data))[2]);
            break;
    }

    return hash;
}


unsigned int hash_crc32(size_t key)
{
    unsigned int hash = 0;

    hash = _mm_crc32_u32(hash, key);

    return hash;
}


