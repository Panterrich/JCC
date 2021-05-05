#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "../Onegin/Onegin.h"
#include "Lists_pointer.hpp"

template <typename ElemT>
struct hash_func {typedef unsigned int (*func)(ElemT);};

template <typename T>
struct remove_reference {};

template <typename T>
struct remove_reference<T&> {typedef T type;};

template <typename T>
struct remove_reference<T&&> {typedef T type;};

template <typename KeyT, typename DataT>
class Hash_table
{
    private:
        List_pointer<KeyT, DataT>* keys_;
        size_t size_;
        typename hash_func<KeyT>::func hash_;
    
    public:
        Hash_table(size_t size, typename hash_func<KeyT>::func hash = nullptr);
       ~Hash_table();

        Hash_table(const Hash_table<KeyT, DataT>& that);
        Hash_table<KeyT, DataT>& operator=(Hash_table<KeyT, DataT> that);

        Hash_table(Hash_table<KeyT, DataT>&& that);
        Hash_table<KeyT, DataT>& operator=(Hash_table<KeyT, DataT>&& that);

        void swap(Hash_table<KeyT, DataT>& that);

        size_t get_size();

        unsigned int hash(KeyT key);
        void set_func(typename hash_func<KeyT>::func hash);
        DataT get_value(KeyT key);
        unsigned int set_value(KeyT key, DataT data);
        void remove(KeyT key);
        void clear();     
};


unsigned int hash_crc(char* key);

#endif