#ifndef LISTS_POINTER_H
#define LISTS_POINTER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <../../Compiler/compiler.h>

//=======================================================================================================================

template <typename ElemT> 
struct Type_traits 
{};

template <> struct Type_traits <char*>
{
    static char* Poison() {return nullptr;};
    static bool IsPoison(char* value) {return value == Type_traits<char*>::Poison();};
    static void Printf(FILE* file, char* value) {fprintf(file, "%s", value);};
};

template <> struct Type_traits <double>
{
    static double Poison() {return NAN;};
    static bool IsPoison(double value) {return isnan(value);};
    static void Printf(FILE* file, double value) {fprintf(file, "%lg", value);};
};

template <> struct Type_traits <int>
{
    static int Poison() {return 0xBAADDEED;};
    static bool IsPoison(int value) {return value == Type_traits<int>::Poison();};
    static void Printf(FILE* file, int value) {fprintf(file, "%d", value);};
};

template <> struct Type_traits <size_t>
{
    static size_t Poison() {return 0xBAADDEED;};
    static bool IsPoison(size_t value) {return value == Type_traits<size_t>::Poison();};
    static void Printf(FILE* file, size_t value) {fprintf(file, "%ld", value);};
};

template <> struct Type_traits <struct Variable>
{
    static struct Variable Poison() {struct Variable temp = {INT32_MAX, 0}; return temp;};
    static bool IsPoison(struct Variable value) {return value.number == Type_traits<struct Variable>::Poison().number;};
    static void Printf(FILE* file, struct Variable value) {fprintf(file, "%ld %d", value.number, value.initialization);};
};

#define LIST_POINTER(name, KeyT, DataT) List_pointer<KeyT, DataT> name(#name)

//======================================================================================================================

template <typename KeyT, typename DataT>
struct List_pointer_t
{
    KeyT  key;
    DataT value;

    List_pointer_t* next;
    List_pointer_t* prev;

};

template <typename KeyT, typename DataT>
class List_pointer
{
    private:
        const char* name_;
    
        size_t size_;

        List_pointer_t<KeyT, DataT>* head_;
        List_pointer_t<KeyT, DataT>* tail_;

    public:
        List_pointer(const char* name = "noname");
       ~List_pointer();

        size_t get_size();

        void clear();


        List_pointer_t<KeyT, DataT>* find_element(size_t number);
        List_pointer_t<KeyT, DataT>* find_value(KeyT key);

        List_pointer_t<KeyT, DataT>* insert_first(KeyT key, DataT value);
        List_pointer_t<KeyT, DataT>* insert_before(size_t number, KeyT key, DataT value);
        List_pointer_t<KeyT, DataT>* insert_after(size_t number, KeyT key, DataT value);
        List_pointer_t<KeyT, DataT>* insert_begin(KeyT key, DataT value);
        List_pointer_t<KeyT, DataT>* insert_end(KeyT key, DataT value);

        void delete_element(size_t number);
        void delete_element(List_pointer_t<KeyT, DataT>* element);

        List_pointer_t<KeyT, DataT>* head();
        List_pointer_t<KeyT, DataT>* tail();
        List_pointer_t<KeyT, DataT>* next(List_pointer_t<KeyT, DataT>* current_element);
        List_pointer_t<KeyT, DataT>* prev(List_pointer_t<KeyT, DataT>* current_element);

        void graph();
};

#endif