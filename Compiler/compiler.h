#ifndef COMPILER_H 
#define COMPILER_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <elf.h>
#include <limits.h>
#include <immintrin.h>

//=============================================================================================================

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"
#include "../libr/hashtable/Hash_table.hpp"
#include "../libr/LibEnd.h"

#include "DSL.h"

//=============================================================================================================

const int MAX_SIZE_CMD               = 40;
const int MAX_SIZE_HASH_TABLE_FUNC   = 100;
const int MAX_SIZE_HASH_TABLE_VARS   = 50;
const int MAX_SIZE_HASH_TABLE_LABELS = 500;

static const char BUFFER[1 << 20] = {};

//=============================================================================================================
struct Registers
{
    bool xmm0_save;
    bool xmm1_save;

    int xmm0_rsp;
    int xmm1_rsp;

    int xmm0_count;
    int xmm1_count;

    int xmm0_rip;
    int xmm1_rip;

    bool flag;
};

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

void Initialization_global_vars(struct Tree* tree, struct Code* info, char* bytecode, struct Registers* reg);

void Translation(struct Tree* tree, struct Code* info, char* bytecode, struct Registers* reg);

size_t Add_ELF_header(FILE* file);

size_t Add_program_header(FILE* file);

void Add_program_code_header(FILE* file, struct Code* info);

void Add_program_data_header(FILE* file, struct Code* info);

void Write_content_elf(FILE* file, struct Code* info, char* lib, char* bytecode);

void Bytecode_alignment(FILE* file, int size);

//=====================================================================================================================

void Print_dec(struct Tree* tree, struct Code* info, char* bytecode, struct Registers* reg);

void Print_func(struct Tree* tree, struct Node* current_node, struct Code* info, char* bytecode, struct Registers* reg);

void Print_body(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_op(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_print(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_printf(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_scan(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_if(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_while(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_assign(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_call(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_ret(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_equation(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_arg(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

void Print_call(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg);

//=====================================================================================================================

int Find_main(struct Tree* tree);

void Find_locale_var(struct Tree* tree, struct Node* current_node, struct Code* info);


#endif