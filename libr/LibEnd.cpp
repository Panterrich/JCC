#include "LibEnd.h"

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(text != nullptr);

    size_t number_line = 0;

    tree->name_equation = Get_name_file(name_program);
    tree->root = Node_create(tree, nullptr, text, &number_line);
}

struct Node* Node_create(struct Tree* tree, struct Node* previous_node, struct Text* text, size_t* number_line)
{
    char* begin = (text->lines)[*number_line].str;
    Skip_separator(&begin);

    if (*begin == '{')
    {
        ++(*number_line);
        begin = (text->lines)[*number_line].str;
        Skip_separator(&begin);

        char* word = Get_word(begin);
        
        if (strcmp(word, "nil") == 0)
        {
            free(word);

            ++(*number_line);
            begin = (text->lines)[*number_line].str;
            Skip_separator(&begin);

            if (*begin == '}') ++(*number_line);
            else
            {
                printf("Syntax error: line #%d\n", *number_line);
            }

            return nullptr;
        }

        else 
        {   
            struct Node* current_node = nullptr;

            if (strcmp(word, "DEC") == 0)
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    DECLARATE, word, NAN, previous_node, 
                                                    nullptr);
            }

            else if (strcmp(word, "LR") == 0)
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    LR, word, NAN, previous_node, 
                                                    nullptr);
            }

            else if (word[0] == '$')
            {   
                current_node = Node_create_and_fill(nullptr, 
                                                    FUNC, Name_key_word(word + 1), Code_key_word(word + 1), previous_node, 
                                                    nullptr);
                free(word);
            }

            else if ((word[0] == '-' && isdigit(word[1])) || isdigit(word[0]))
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    NUMBER, nullptr, strtod(begin, &begin), previous_node,
                                                    nullptr);
                free(word);
            }

            else if (Is_key_word(word))
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    Type_key_word(word), Name_key_word(word), Code_key_word(word), previous_node,
                                                    nullptr);
                
                free(word);
            }

            else 
            {
                current_node = Node_create_and_fill(nullptr, 
                                                    VAR, word, NAN, previous_node,
                                                    nullptr);
            }

            ++(*number_line);
            ++(tree->size);

            current_node->left  = Node_create(tree, current_node, text, number_line);
            current_node->right = Node_create(tree, current_node, text, number_line);

            begin = (text->lines)[*number_line].str;
            Skip_separator(&begin);

            if (*begin == '}') ++(*number_line);
            else
            {
                printf("Syntax error: line #%d\n", *number_line);
            }

            return current_node;
        }
    }

    return nullptr;
}

char* Get_word(char* begin)
{
    char* current_symbol = begin;

    while (!isspace(*current_symbol) && *current_symbol != '\0' && *current_symbol != '\n') ++current_symbol;

    return strndup(begin, current_symbol - begin); 
}

char* Name_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return strdup(word);

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return strdup(str);
}

int Type_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return type;

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return 0;
}

double Code_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return value;

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return NAN;
}

int Is_key_word(char* str)
{
    #define KEY_WORD(replace, len, word, type, value, number, path) if (strcmp(word, str) == 0) return 1;

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD

    return 0;
}


char* Get_name_file(const char* file)
{
    assert(file != nullptr);

    const char* begin = file;
    while ((strchr(begin, '/')) != nullptr) begin = strchr(begin, '/') + 1;
    char* name_file   = strdup(begin);

    char* pointer_format  = strchr(name_file, '.');
    if   (pointer_format != nullptr) *pointer_format = '\0';

    return name_file;
}


size_t Skip_separator(char** string)
{   
    assert(string != nullptr);

    size_t count = 0;

    while (isspace((int)(unsigned char)**string) || **string == '\n')
    {
        ++(*string);
        ++count;
    }

    return count; 
}


void Tree_print(struct Tree* tree)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    char name_output[MAX_SIZE_COMMAND] = {};
    sprintf(name_output, "files/%s.me", tree->name_equation);

    FILE* file = fopen(name_output, "w");
    
    Node_print(tree->root, file);

    fclose(file);

    char command[MAX_SIZE_COMMAND] = {};
    sprintf(command, "astyle --mode=cs --suffix=none --style=ansi %s", name_output);
    system(command);
}

void Node_print(struct Node* current_node, FILE* file)
{
    assert(file != nullptr);

    if (current_node == nullptr) 
    {
        fprintf(file, "{\nnil\n}\n");
        return;
    }

    if (current_node->type == FUNC)
    {    
        fprintf(file, "{\n$%s\n", current_node->str);
    }

    else if (current_node->type == NUMBER)
    {
        fprintf(file, "{\n%lg\n", current_node->value);
    }

    else
    {
        fprintf(file, "{\n%s\n", current_node->str);
    }

    Node_print(current_node->left, file);
    Node_print(current_node->right,  file);

    fprintf(file, "}\n");
}
