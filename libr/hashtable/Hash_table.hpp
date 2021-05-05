#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "Hash_table.h"

template <typename KeyT, typename DataT>
Hash_table <KeyT, DataT> :: Hash_table(size_t size, typename hash_func<KeyT>::func hash) :
    size_(size),
    hash_(hash),
    keys_(nullptr)
{
    keys_ = new List_pointer<KeyT, DataT>[size];
}

template <typename KeyT, typename DataT>
Hash_table <KeyT, DataT> :: ~Hash_table()
{
    delete[] keys_;

    size_ = 0;
    hash_ = nullptr;
    keys_ = nullptr;
}

template <typename KeyT, typename DataT>
Hash_table <KeyT, DataT> :: Hash_table(const Hash_table<KeyT, DataT>& that) :
    keys_(that.keys_),
    hash_(that.hash_),
    size_(that.size_)
{
    that.size_ = 0;
    that.keys_ = nullptr;
    that.hash_ = nullptr;
}

template <typename KeyT, typename DataT>
Hash_table <KeyT, DataT> :: Hash_table(Hash_table<KeyT, DataT>&& that) :
    keys_(nullptr),
    hash_(nullptr),
    size_(0)
{
    swap(that);
}

template <typename KeyT, typename DataT>
Hash_table<KeyT, DataT>& Hash_table<KeyT, DataT> :: operator=(Hash_table<KeyT, DataT> that)
{
    if (this == &that) return *this;

    swap(that);
    return *this;
}

template <typename KeyT, typename DataT>
Hash_table<KeyT, DataT>& Hash_table<KeyT, DataT> :: operator=(Hash_table<KeyT, DataT>&& that)
{
    if (this == &that) return *this;

    swap(that);
    return *this;
}

template <typename KeyT, typename DataT>
void Hash_table<KeyT, DataT> :: swap(Hash_table<KeyT, DataT>& that)
{
    if (this != &that)
    {
        Hash_table<KeyT, DataT> temp;

        temp.keys_ = keys_;
        temp.size_ = size_;
        temp.hash_ = hash_;

        keys_ = that.keys_;
        hash_ = that.hash_;
        size_ = that.size_;

        that.keys_ = temp.keys_;
        that.hash_ = temp.hash_;
        that.size_ = temp.size_;
    }
}

template <typename KeyT, typename DataT>
size_t Hash_table <KeyT, DataT> :: get_size()
{
    return size_;
}

template <typename KeyT, typename DataT>
unsigned int Hash_table <KeyT, DataT> :: hash(KeyT key)
{
    if (hash_ == nullptr) return 0;
    return hash_(key);
}

template <typename KeyT, typename DataT>
void Hash_table<KeyT, DataT> :: set_func(typename hash_func<KeyT>::func hash)
{
    hash_ = hash;
}
      
template <typename KeyT, typename DataT>
DataT Hash_table<KeyT, DataT> :: get_value(KeyT key)
{
    if (size_ == 0 || hash_ == nullptr) return Type_traits<DataT>::Poison();

    List_pointer_t<KeyT, DataT>* element = keys_[hash(key) % size_].find_value(key);

    if (element != nullptr)
    {
        return element->value;
    }
    
    else
    {
        return Type_traits<DataT>::Poison();
    }
}

template <typename KeyT, typename DataT>
unsigned int Hash_table<KeyT, DataT> :: set_value(KeyT key, DataT data)
{
    if (size_ == 0 || hash_ == nullptr) return -1;

    unsigned int hash_key = hash(key); 

    keys_[hash_key % size_].insert_end(key, data);

    return hash_key;
}

template <typename KeyT, typename DataT>
void Hash_table<KeyT, DataT> :: remove(KeyT key)
{
    if (size_ == 0 || hash_ == nullptr) return;

    size_t index = hash(key) % size_;
    keys_[index].delete_element(keys_[index].find_value(key));
}
    
template <typename KeyT, typename DataT>
void Hash_table<KeyT, DataT> :: clear()
{
    for (size_t index = 0; index < size_; ++index)
    {
        keys_[index].clear();
    }
}
#endif