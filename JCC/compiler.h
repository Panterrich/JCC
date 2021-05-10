#ifndef COMPILER_H 
#define COMPILER_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <elf.h>
#include <limits.h>
#include <immintrin.h>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//=============================================================================================================

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"
#include "../libr/hashtable/Hash_table.hpp"
#include "../libr/Differentiator/Differentiator.h"

//=============================================================================================================
//#define ALLDIF
//=============================================================================================================

//================================================================================================================

struct Program
{
    char* buffer; 
    char* current_symbol;
};

//================================================================================================================

enum KEY_WORDS
{
    #define KEY_WORD(replace, len, word, type, value, number, path) KEY##_##number = value,

    #ifdef HG
        #include "../libr/key_words_hg.h"
    #else 
        #include "../libr/key_words_rm.h"
    #endif
    #undef KEY_WORD
};

//================================================================================================================

const size_t INITIAL_CAPACITY = 20;

//================================================================================================================

void Program_create(FILE* file, struct Program* program);

void Tokenizer(struct Program* program, struct Stack* nodes);

size_t Skip_separator(char** string);

void Program_destruct(struct Program* program);

void Free_nodes(struct Stack* nodes);

char* Get_name_file(const char* file);

//================================================================================================================

struct Node* Get_node(struct Stack* nodes, size_t index);

struct Node* GetProg(struct Tree* tree, struct Stack* nodes, const char* name_program);

struct Node* GetDec(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetGlobal(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetFunc(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetParam(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node);

struct Node* GetBody(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetOp(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetPrint(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetPrintf(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetScan(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetIf(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetWhile(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetAssign(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetRet(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetCall(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetC(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetE(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetT(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetD(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetP(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetN(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetW(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetGroup(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

struct Node* GetArg(struct Tree* tree, struct Node* previous_node, struct Stack* nodes, size_t* index_node, FILE* list);

void Require(struct Tree* tree, struct Stack* nodes, size_t* index_node, int type, double value);

void Syntax_error(struct Stack* nodes, size_t index_node);

const char* Get_path(int code);

//=============================================================================================================

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program);

struct Node* Node_create(struct Tree* tree, struct Node* previous_node, struct Text* text, size_t* number_line);

char* Get_name_file(const char* file);

char* Get_word(char* begin);

size_t Skip_separator(char** string);

char* Name_key_word(char* str);

int Type_key_word(char* str);

double Code_key_word(char* str);

int Is_key_word(char* str);

void Tree_print(struct Tree* tree);

void Node_print(struct Node* current_node, FILE* file);

struct Node* Tree_search_node(struct Tree* tree, struct Node* current_node, int type, int number);

void Tree_processing(struct Tree* tree);

void All_differentiation(struct Tree* tree, struct Node* current_node);

struct Node* My_derivative(struct Tree* tree, struct Node* current_node);


//=============================================================================================================

const int MAX_SIZE_CMD               = 40;
const int MAX_SIZE_HASH_TABLE_FUNC   = 100;
const int MAX_SIZE_HASH_TABLE_VARS   = 50;
const int MAX_SIZE_HASH_TABLE_LABELS = 500;

static const char BUFFER[1 << 20] = {};

//=============================================================================================================

#define BYTE(c) bytecode[info->rip++] = c; ++info->size_code;

#define BYTE1(c1)                     BYTE(c1);
#define BYTE2(c1, c2)                 BYTE(c1); BYTE(c2);
#define BYTE3(c1, c2, c3)             BYTE(c1); BYTE(c2); BYTE(c3);
#define BYTE4(c1, c2, c3, c4)         BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4);
#define BYTE5(c1, c2, c3, c4, c5)     BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4); BYTE(c5);
#define BYTE6(c1, c2, c3, c4, c5, c6) BYTE(c1); BYTE(c2); BYTE(c3); BYTE(c4); BYTE(c5); BYTE(c6);

#define ADDRESS(ptr)          *((int*)   (&(bytecode[info->rip]))) = ptr; info->rip += 4; info->size_code += 4;
#define NUMBER(n)             *(double*)(&bytecode[info->rip]) = n;   info->rip += 8; info->size_code += 8;

//=============================================================================================================

struct Variable
{
    size_t number;
    int initialization;
};

struct Code
{
    size_t size_lib;
    size_t size_code;
    size_t size_data;
    size_t size_headers;

    size_t offset_data;

    size_t count_var;
    size_t count_label;

    Hash_table<char*, size_t> func;
    Hash_table<char*, struct Variable> global_var;
    Hash_table<char*, struct Variable> locale_var;
    Hash_table<size_t, size_t> label;
    
    size_t rip;
};

//=============================================================================================================

void Create_elf(struct Tree* tree, FILE* file);

char* Include_lib(struct Code* info);

void Compile_pass(struct Tree* tree, struct Code* info, char* bytecode);

void Initialization_global_vars(struct Tree* tree, struct Code* info, char* bytecode);

void Translation(struct Tree* tree, struct Code* info, char* bytecode);

size_t Add_ELF_header(FILE* file);

size_t Add_program_header(FILE* file);

void Add_program_code_header(FILE* file, struct Code* info);

void Add_program_data_header(FILE* file, struct Code* info);

void Write_content_elf(FILE* file, struct Code* info, char* lib, char* bytecode);

void Bytecode_alignment(FILE* file, int size);

//=====================================================================================================================

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program);

struct Node* Node_create(struct Tree* tree, struct Node* previous_node, struct Text* text, size_t* number_line);

char* Get_name_file(const char* file);

char* Get_word(char* begin);

size_t Skip_separator(char** string);

char* Name_key_word(char* str);

int Type_key_word(char* str);

double Code_key_word(char* str);

int Is_key_word(char* str);

//=====================================================================================================================

void Print_dec(struct Tree* tree, struct Code* info, char* bytecode);

void Print_func(struct Tree* tree, struct Node* current_node, struct Code* info, char* bytecode);

void Print_body(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_op(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_print(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_printf(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_scan(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_if(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_while(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_assign(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_call(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_ret(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_equation(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_arg(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

void Print_call(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode);

//=====================================================================================================================

int Find_main(struct Tree* tree);

void Find_locale_var(struct Tree* tree, struct Node* current_node, struct Code* info);

//======================================================================================================================

int Compile(const char* name_program);

int Create_window(sf:: Music* music, sf:: Music* joke_music);

void Set_text(sf::Text* text, const sf::Font& font, const sf::Color color, size_t font_size, float x, float y);

void Set_text(sf::Text* text, const sf::Font& font, const sf::Color color, size_t font_size, float x, float y, const char* str);

#endif