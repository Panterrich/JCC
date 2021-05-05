#ifndef LISTS_POINTER_HPP
#define LISTS_POINTER_HPP

#include "Lists_pointer.h"

template <typename KeyT, typename DataT>
List_pointer <KeyT, DataT> :: List_pointer(const char* name) :
    name_(name),
    size_(0),
    head_(nullptr),
    tail_(nullptr)
{}

template <typename KeyT, typename DataT>
List_pointer<KeyT, DataT> :: ~List_pointer()
{
    assert(this != nullptr);

    clear();

    size_ = -1;
}

template <typename KeyT, typename DataT>
size_t List_pointer<KeyT, DataT> :: get_size()
{
    return size_;
}

template <typename KeyT, typename DataT>
void List_pointer<KeyT, DataT> :: clear()
{
    assert(this != nullptr);

    List_pointer_t<KeyT, DataT>* current_element = tail();
    List_pointer_t<KeyT, DataT>* next_element    = nullptr;

    for (size_t index = 0; index < size_; ++index)
    {
        next_element = current_element->prev;

        current_element->prev  = nullptr;
        current_element->next  = nullptr;
        current_element->value = Type_traits<DataT>::Poison();
        current_element->key   = Type_traits<KeyT>::Poison();

        free(current_element);

        current_element = next_element;
    }

    size_ = 0;
    head_ = nullptr;
    tail_ = nullptr;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: insert_first(KeyT key, DataT value)
{
    assert(this != nullptr);

    List_pointer_t<KeyT, DataT>* new_element = (List_pointer_t<KeyT, DataT>*) calloc(1, sizeof(List_pointer_t<KeyT, DataT>));

    new_element->value = value;
    new_element->key   = key;
    new_element->prev  = nullptr;
    new_element->next  = nullptr;

    size_ = 1;
    head_ = new_element;
    tail_ = new_element; 

    return new_element;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: find_element(size_t number)
{   
    assert(this != nullptr);

    List_pointer_t<KeyT, DataT>* current_element = head();

    for (size_t index = 1; index < number; ++index)
    {
        current_element = next(current_element);
    }

    return current_element;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: find_value(KeyT key)
{
    assert(this != nullptr);

    List_pointer_t<KeyT, DataT>* current_element = head();

    for (size_t index = 1; index <= size_; ++index)
    {
        if (current_element->key == key)
        {
            return current_element;
        }

        current_element = next(current_element);
    }

    return nullptr;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: insert_before(size_t number, KeyT key, DataT value)
{
    assert(this != nullptr);

    if (size_ == 0)
    {
        return insert_first(value);
    }

    if (number == 1)
    {
        return insert_begin(value);
    }

    List_pointer_t<KeyT, DataT>* current_element = find_element(number);
    List_pointer_t<KeyT, DataT>* new_element = (List_pointer_t<KeyT, DataT>*) calloc (1, sizeof(List_pointer_t<KeyT, DataT>));

    new_element->prev  = current_element->prev;
    new_element->next  = current_element;
    new_element->value = value;
    new_element->key   = key;

    prev(current_element)->next = new_element;
    current_element->prev = new_element;

    ++size_;
    
    return new_element;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: insert_after(size_t number, KeyT key, DataT value)
{
    assert(this != nullptr);

    if (size_ == 0)
    {
        return insert_first(key, value);
    }

    if (number == size_)
    {
        return insert_end(key, value);
    }

    List_pointer_t<KeyT, DataT>* current_element = find_element(number);
    List_pointer_t<KeyT, DataT>* new_element = (List_pointer_t<KeyT, DataT>*) calloc (1, sizeof(List_pointer_t<KeyT, DataT>));

    new_element->next  = current_element->next;
    new_element->prev  = current_element;
    new_element->value = value;
    new_element->key   = key;

    next(current_element)->prev = new_element;
    current_element->next = new_element;

    ++size_;
    
    return new_element;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: insert_begin(KeyT key, DataT value)
{
    assert(this != nullptr);
    
    if (size_ == 0)
    {
        return insert_first(key, value);
    }

    List_pointer_t<KeyT, DataT>* current_element = head();
    List_pointer_t<KeyT, DataT>* new_element = (List_pointer_t<KeyT, DataT>*) calloc (1, sizeof(List_pointer_t<KeyT, DataT>));

    new_element->prev  = nullptr;
    new_element->next  = current_element;
    new_element->value = value;
    new_element->key   = key;

    current_element->prev = new_element;

    head_ = new_element;

    ++size_;
    
    return new_element;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: insert_end(KeyT key, DataT value)
{
    assert(this != nullptr);

    if (size_ == 0)
    {
        return insert_first(key, value);
    }

    List_pointer_t<KeyT, DataT>* current_element = tail();
    List_pointer_t<KeyT, DataT>* new_element = (List_pointer_t<KeyT, DataT>*) calloc (1, sizeof(List_pointer_t<KeyT, DataT>));

    new_element->prev  = current_element;
    new_element->next  = nullptr;
    new_element->value = value;
    new_element->key   = key;

    current_element->next = new_element;

    tail_ = new_element;

    ++size_;

    return new_element;
}

template <typename KeyT, typename DataT>
void List_pointer<KeyT, DataT> :: delete_element(size_t number)
{
    assert(this != nullptr);

    List_pointer_t<KeyT, DataT>* current_element = find_element(number);

    if (size_ == 1)
    {
        head_ = nullptr;
        tail_ = nullptr;
    }

    else if (current_element == head_)
    {
        head_ = next(current_element);
        next(current_element)->prev = nullptr;
    }

    else if (current_element == tail_)
    {
        tail_ = prev(current_element);
        prev(current_element)->next = nullptr;
    } 

    else
    {
        prev(current_element)->next = next(current_element);
        next(current_element)->prev = prev(current_element);
    }
    
    current_element->next  = nullptr;
    current_element->prev  = nullptr;
    current_element->value = Type_traits<DataT>::Poison();
    current_element->key   = Type_traits<KeyT>::Poison();

    free(current_element);

    --size_;
}

template <typename KeyT, typename DataT>
void List_pointer<KeyT, DataT> :: delete_element(List_pointer_t<KeyT, DataT>* element)
{
    assert(this != nullptr);

    if (size_ == 1)
    {
        head_ = nullptr;
        tail_ = nullptr;
    }

    else if (element == head_)
    {
        head_ = next(element);
        next(element)->prev = nullptr;
    }

    else if (element == tail_)
    {
        tail_ = prev(element);
        prev(element)->next = nullptr;
    } 

    else
    {
        prev(element)->next = next(element);
        next(element)->prev = prev(element);
    }
    
    element->next  = nullptr;
    element->prev  = nullptr;
    element->value = Type_traits<DataT>::Poison();
    element->key   = Type_traits<KeyT>::Poison();

    free(element);

    --size_;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: head()
{
    assert(this != nullptr);

    return head_;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: tail()
{
    assert(this != nullptr);

    return tail_;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: next(List_pointer_t<KeyT, DataT>* current_element)
{
    assert(this            != nullptr);
    assert(current_element != nullptr);

    return current_element->next;
}

template <typename KeyT, typename DataT>
List_pointer_t<KeyT, DataT>* List_pointer<KeyT, DataT> :: prev(List_pointer_t<KeyT, DataT>* current_element)
{
    assert(this            != nullptr);
    assert(current_element != nullptr);

    return current_element->prev;
}

template <typename KeyT, typename DataT>
void List_pointer<KeyT, DataT> :: graph()
{
    assert(this != nullptr);

    FILE* text = fopen("graph_pointer.dot", "w");

    fprintf(text, "digraph G {\n"            
                  "graph [bgcolor = Snow2]\n" 
                  "rankdir = LR;\n");

    List_pointer_t<KeyT, DataT>* current_element = head();
   
    for (size_t index = 1; index <= size_; ++index)
    {   
        if (!Type_traits<KeyT>::IsPoison(current_element->key))
        {
           fprintf(text, "\t%lu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> ", index); 

            Type_traits<KeyT>::Printf(text, current_element->key);
            fprintf(text, " | ");
            Type_traits<DataT>::Printf(text, current_element->value);


            fprintf(text, " </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %p </td>\n"
                          "\t\t <td> %p </td>\n"
                          "\t\t <td> %p </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = deepskyblue2, fillcolor = lightskyblue]\n", 
                    current_element->prev, current_element, current_element->next);
        }

        else
        {
            fprintf(text, "\t%lu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\">", index);

            Type_traits<KeyT>::Printf(text, current_element->key);
            fprintf(text, " | ");
            Type_traits<DataT>::Printf(text, current_element->value);

            fprintf(text, " </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %p </td>\n"
                          "\t\t <td> %p </td>\n"
                          "\t\t <td> %p </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = red,  fillcolor = lightcoral]\n", 
                    current_element->prev, current_element, current_element->next);
        }

        current_element = next(current_element);
    }
    
    current_element = head();

    for (size_t index = 1; index < size_; ++index)
    {   
        if (!Type_traits<KeyT>::IsPoison(current_element->key))
        {
            if ((!Type_traits<KeyT>::IsPoison((next(current_element)->key))) && (next(current_element)->prev == current_element))
            {
                fprintf(text, "\t%ld -> %ld [color = deepskyblue2]; \n", index, index + 1);
                fprintf(text, "\t%ld -> %ld [color = deepskyblue2]; \n", index + 1, index);
            }

            else if  ((next(current_element) == nullptr) && (current_element == tail_))
            {
                ;
            }

            else 
            {
                fprintf(text, "\t%ld -> %ld [color = darkred]; \n", index, index + 1);
                fprintf(text, "\t%ld -> %ld [color = darkred]; \n", index + 1, index);
            }
        }

        else 
        {
            fprintf(text, "\t%ld -> %ld [color = darkred]; \n", index, index + 1);
            fprintf(text, "\t%ld -> %ld [color = darkred]; \n", index + 1, index);
        }

        current_element = next(current_element);
    }

    fprintf(text, "\tHEAD [style = filled, color = black, fillcolor = salmon1]; \n");
    fprintf(text, "\tTAIL [style = filled, color = black, fillcolor = salmon1]; \n");
    
    fprintf(text, "\tHEAD -> %ld [color = deeppink]; \n", (size_ != 0));
    fprintf(text, "\tTAIL -> %ld [color = deeppink]; \n", size_);

    fprintf(text, "}\n");

    fclose(text);

    system("dot -Tjpeg -ograph_pointer.jpeg graph_pointer.dot");
    system("gwenview graph_pointer.jpeg");
}

#endif