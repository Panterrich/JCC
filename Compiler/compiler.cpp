#include "compiler.h"

template <> struct Type_traits <struct Variable>
{
    static struct Variable Poison() {struct Variable temp = {SIZE_MAX, 0}; return temp;};
    static bool IsPoison(struct Variable value) {return value.number == Type_traits<struct Variable>::Poison().number;};
    static void Printf(FILE* file, struct Variable value) {fprintf(file, "%ld %d", value.number, value.initialization);};
};

void Create_elf(struct Tree* tree, FILE* file)
{
    struct Code info = {0, 0, 0, 0, 0, 0, 0, {MAX_SIZE_HASH_TABLE_FUNC,   hash_crc},     \
                                             {MAX_SIZE_HASH_TABLE_VARS,   hash_crc},     \
                                             {MAX_SIZE_HASH_TABLE_VARS,   hash_crc},     \
                                             {MAX_SIZE_HASH_TABLE_LABELS, hash_crc32}, 0};

    char* lib      = Include_lib(&info);
    char* bytecode = (char*) calloc(tree->size * MAX_SIZE_CMD, sizeof(char));

    if (!Find_main(tree)) 
    {
        tree->error = NO_MAIN_DECLARATION;
        TREE_ASSERT_OK(tree);
    }

    Compile_pass(tree, &info, bytecode);

    info.size_data    += 1 << 20;

    info.size_headers += Add_ELF_header(file);
    info.size_headers += Add_program_header(file);

    Add_program_code_header(file, &info);
    Add_program_data_header(file, &info);
    
    info.count_label = 0;
    info.rip         = 0;
    info.size_code   = 0;
    info.size_data   = 0;

    Compile_pass(tree, &info, bytecode);

    info.size_data    += 1 << 20;

    Write_content_elf(file, &info, lib, bytecode);
}

char* Include_lib(struct Code* info)
{
    assert(info);

    FILE* lib = fopen("libr/IOlib", "rb");

    size_t n_lines = 0;  // PLUG
    char* lib_buffer = Create_buffer(lib, &n_lines, &info->size_lib);

    fclose(lib);

    return lib_buffer;
}

void Compile_pass(struct Tree* tree, struct Code* info, char* bytecode)
{
    assert(tree);
    assert(info);
    assert(bytecode);

    BYTE3(0x49, 0xC7, 0xC6); // mov r14, data + var * 8;
    ADDRESS((info->offset_data * 0x1000 + 0x401000 + (info->global_var.get_count() << 3)));

    BYTE3(0x49, 0xC7, 0xC7); // mov r15, data
    ADDRESS(info->offset_data * 0x1000 + 0x401000);

    struct Registers reg = {};

    Initialization_global_vars(tree, info, bytecode, &reg);
    Translation(               tree, info, bytecode, &reg);
}

void Initialization_global_vars(struct Tree* tree, struct Code* info, char* bytecode, struct Registers* reg)
{
    assert(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    struct Node* current_node = tree->root;

    while (current_node->type == DECLARATE)
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == OPERATION && current_node->left->value == 32)
            {
                struct Node* new_var = current_node->left->left;
                struct Variable var = info->global_var.get_value(new_var->str);

                if (Type_traits<struct Variable>::IsPoison(var))
                {
                    var.number         = info->global_var.get_count();
                    var.initialization = 1;

                    info->global_var.set_value(new_var->str, var);
                    ++info->count_var;
                    info->size_data += 8;
                }

                Print_equation(tree, current_node->left->right, info, 0, bytecode, reg);
                CLEAR_XMM0();

                if (var.number == 0) // pop [r15 + (number << 3)]
                {
                    BYTE3(0x41, 0x8F, 0x07); 
                }

                else if (var.number > 0 && var.number < 16) 
                {
                    BYTE4(0x41, 0x8f, 0x47, var.number << 3);
                }

                else
                {
                    BYTE3(0x41, 0x8F, 0x87);
                    ADDRESS(var.number << 3);
                }
            }
        }

        current_node = current_node->right;

        if (current_node == nullptr) break;
    }

    if (current_node != nullptr)
    {   
        if (current_node->type == OPERATION && current_node->value == 32)
        {
            struct Node* new_var = current_node->left;
            struct Variable var = info->global_var.get_value(new_var->str);

            if (Type_traits<struct Variable>::IsPoison(var))
            {
                var.number         = info->global_var.get_count();
                var.initialization = 1;

                info->global_var.set_value(new_var->str, var);
                ++info->count_var;
                info->size_data += 8;
            }

            Print_equation(tree, current_node->right, info, 0, bytecode, reg);
            CLEAR_XMM0();

            if (var.number == 0) // pop [r15 + (number << 3)]
            {
                BYTE3(0x41, 0x8F, 0x07);
            }

            else if (var.number > 0 && var.number < 16) 
            {
                BYTE4(0x41, 0x8f, 0x47, var.number << 3);
            }

            else
            {
                BYTE3(0x41, 0x8F, 0x87);
                ADDRESS(var.number << 3);
            }
        }
    }
}

void Translation(struct Tree* tree, struct Code* info, char* bytecode, struct Registers* reg)
{
    assert(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    size_t shift_main = info->func.get_value(const_cast<char*>("main"));

    BYTE1(0xE8); // call main
    ADDRESS(shift_main - (info->rip + 4));

    BYTE1(0xE8); // jmp hlt
    ADDRESS(0x1C1 - (info->size_lib + info->rip + 4)); // 0x1C1 - begin func hlt

    Print_dec(tree, info, bytecode, reg);
}

size_t Add_ELF_header(FILE* file)
{
    assert(file);

    Elf64_Ehdr header = {};

    header.e_ident[EI_MAG0] = ELFMAG0;
    header.e_ident[EI_MAG1] = ELFMAG1;
    header.e_ident[EI_MAG2] = ELFMAG2;  
    header.e_ident[EI_MAG3] = ELFMAG3; // Building the elf signature: 0x7f 'E' 'L' 'F'
    
    header.e_ident[EI_CLASS]   = ELFCLASS64;    // 64-bit format
    header.e_ident[EI_DATA]    = ELFDATA2LSB;   // Big Endian
    header.e_ident[EI_VERSION] = EV_CURRENT;  
    header.e_ident[EI_OSABI]   = ELFOSABI_NONE;

    // We will ignore the other elements of e_ident

    header.e_type      = ET_EXEC;    // Executable file 
    header.e_machine   = EM_X86_64;  // x86-64 machine 
    header.e_version   = EV_CURRENT; 
    header.e_entry     = 0x401000;   // Entry point
    header.e_phoff     = 64;         // Points to the start of the program header table.
    header.e_shoff     = 0x0;        // Points to the start of the section header table.
    header.e_flags     = 0x0;        // Interpretation of this field depends on the target architecture.
    header.e_ehsize    = 64;         // Contains the size of this header, normally 64 Bytes for 64-bit and 52 Bytes for 32-bit format.
    header.e_phentsize = 56;         // Contains the size of a program header table entry.
    header.e_phnum     = 3;          // Contains the number of entries in the program header table. 

    header.e_shentsize = 64;         // Contains the size of a section header table entry.
    header.e_shnum     = 0;          // Contains the number of entries in the section header table.
    header.e_shstrndx  = 0;          // Contains index of the section header table entry that contains the section names.

    fwrite(&header, sizeof(unsigned char), sizeof(header), file);

    return sizeof(header);
}

size_t Add_program_header(FILE* file)
{
    assert(file);

    Elf64_Phdr header = {};

    header.p_type   = PT_LOAD;
    header.p_flags  = PF_R;

    header.p_offset = 0x0;
    header.p_vaddr  = 0x400000;
    header.p_paddr  = 0x400000;
    header.p_filesz = 0xE8;
    header.p_memsz  = 0xE8;

    header.p_align  = 0x1000;

    fwrite(&header, sizeof(unsigned char), sizeof(header), file);

    return 3 * sizeof(header);
}

void Add_program_code_header(FILE* file, struct Code* info)
{
    assert(file);
    assert(info);

    Elf64_Phdr header = {};

    header.p_type   = PT_LOAD;
    header.p_flags  = PF_X | PF_R;

    header.p_offset = 0x1000;
    header.p_vaddr  = 0x401000;
    header.p_paddr  = 0x401000;
    header.p_filesz = info->size_code + info->size_lib;
    header.p_memsz  = info->size_code + info->size_lib;

    header.p_align  = 0x1000;

    fwrite(&header, sizeof(unsigned char), sizeof(header), file);

    info->offset_data = ((0x401000 + info->size_code + info->size_lib) >> 12) - 0x400;
}


void Add_program_data_header(FILE* file, struct Code* info)
{
    assert(file);
    assert(info);

    Elf64_Phdr header = {};

    header.p_type   = PT_LOAD;
    header.p_flags  = PF_R | PF_W;

    header.p_offset = info->offset_data * 0x1000 + 0x1000;
    header.p_vaddr  = info->offset_data * 0x1000 + 0x401000;
    header.p_paddr  = info->offset_data * 0x1000 + 0x401000;
    header.p_filesz = info->size_data;
    header.p_memsz  = info->size_data;

    header.p_align  = 0x1000;

    fwrite(&header, sizeof(unsigned char), sizeof(header), file);
}

void Write_content_elf(FILE* file, struct Code* info, char* lib, char* bytecode)
{
    assert(file);
    assert(info);
    assert(bytecode);   

    Bytecode_alignment(file, 0x1000 - info->size_headers);

    fwrite(lib,      info->size_lib,  sizeof(char), file);
    fwrite(bytecode, info->size_code, sizeof(char), file);

    Bytecode_alignment(file, ((info->size_code + info->size_lib >> 12) + 1 << 12) - info->size_lib - info->size_code);

    //Data
    Bytecode_alignment(file, info->size_data);

}

void Bytecode_alignment(FILE* file, int size)
{
    assert(file);

    if (size < 0)
    {
        printf("ERROR: negative size in bytecode alignment function \n");
    }

    else
    {
        fwrite(BUFFER, size, sizeof(char), file);
    }

}

//=============================================================================================================

void Print_dec(struct Tree* tree, struct Code* info, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    struct Node* current_node = tree->root;

    while (current_node->type == DECLARATE)
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == FUNC) Print_func(tree, current_node->left, info, bytecode, reg);
        }

        current_node = current_node->right;

        if (current_node == nullptr) break;
    }

    if (current_node != nullptr)
    {   
        if (current_node->type == FUNC) Print_func(tree, current_node, info, bytecode, reg);
    }
}

void Print_func(struct Tree* tree, struct Node* current_node, struct Code* info, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    Find_locale_var(tree, current_node->left, info);
    size_t count_param = info->locale_var.get_count();

    info->locale_var.visitor();

    Find_locale_var(tree, current_node->right, info);
    size_t count_var = info->locale_var.get_count();

    info->func.set_value(current_node->str, info->rip);

    BYTE2(0x41, 0x5c); // pop  r12

    for (size_t index = 0; index < count_param; ++index)
    {
        if (index == 0)
        {
            BYTE3(0x41, 0x8F, 0x06);  // pop [r14 + 0]
        }

        else if (index > 0 && index < 16)
        {
            BYTE4(0x41, 0x8F, 0x46, index << 3);
        }

        else
        {
            BYTE3(0x41, 0x8F, 0x86);
            ADDRESS(index << 3);
        }
    }

    Print_body(tree, current_node->right, info, count_var, bytecode, reg);

    BYTE2(0x41, 0x54); // push r12
    BYTE1(0xC3); // ret

    info->locale_var.clear();
}

void Print_body(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    if (current_node->type == LR) 
    {
        Print_op(  tree, current_node->left,  info, count_var, bytecode, reg);
        Print_body(tree, current_node->right, info, count_var, bytecode, reg);
    }
}

void Print_op(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    if (current_node->type == LR && current_node->value == 3)
    {
        return;
    }

    if (current_node->type == FUNC && current_node->value == 10)
    {
        Print_print(tree, current_node, info, count_var, bytecode, reg);
        return;
    }

    if (current_node->type == FUNC && current_node->value == 12)
    {
        Print_printf(tree, current_node, info, count_var, bytecode, reg);
        return;
    }

    if (current_node->type == FUNC && current_node->value == 11)
    {
        Print_scan(tree, current_node, info, count_var, bytecode, reg);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 30)
    {   
        Print_if(tree, current_node, info, count_var, bytecode, reg);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 31)
    {
        Print_while(tree, current_node, info, count_var, bytecode, reg);
        return;
    }

    if (current_node->type == OPERATION && current_node->value == 32)
    {
        Print_assign(tree, current_node, info, count_var, bytecode, reg);
        return;
    }

    if (current_node->type == FUNC)
    {
        Print_call(tree, current_node, info, count_var, bytecode, reg);

        BYTE4(0x48, 0x83, 0xC4, 0x08); // add rsp, 8
                                       // remove returned value
        return;
    }
    
    Print_ret(tree, current_node, info, count_var, bytecode, reg);
    return;
}

void Print_print(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    Print_equation(tree, current_node->left, info, count_var, bytecode, reg);
    CLEAR_XMM0();

    BYTE1(0xE8); // call print
    ADDRESS(0x182 - (info->size_lib + info->rip + 4));  

    BYTE4(0x48, 0x83, 0xC4, 0x08); // add rsp, 8
}

void Print_printf(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    Print_equation(tree, current_node->left, info, count_var, bytecode, reg);
    CLEAR_XMM0();

    BYTE1(0xE8); // call printf
    ADDRESS(0xB4 - (info->size_lib + info->rip + 4));   

    BYTE4(0x48, 0x83, 0xC4, 0x08); // add rsp, 8
}

void Print_scan(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    struct Variable var = info->global_var.get_value(current_node->left->str);
    
    if (!Type_traits<struct Variable>::IsPoison(var))
    {
        BYTE1(0xE8); // call scan
        ADDRESS(0x05 - (info->size_lib + info->rip + 4));

        if (var.number == 0) //
        {
            BYTE3(0x41, 0x8F, 0x07); 
        }

        else if (var.number > 0 && var.number < 16)
        {
            BYTE4(0x41, 0x8F, 0x47, var.number << 3);
        }

        else
        {
            BYTE3(0x41, 0x8F, 0x87);
            ADDRESS(var.number << 3);
        }
    }

    else
    {
        struct Variable loc_var = info->locale_var.get_value(current_node->left->str);

        if (Type_traits<struct Variable>::IsPoison(loc_var))
        {
            tree->error = VAR_NOT_FOUND;
            TREE_ASSERT_OK(tree);
        }

        info->locale_var.remove(current_node->left->str);
        info->locale_var.set_value(current_node->left->str, {loc_var.number, 1});

        BYTE1(0xE8); // call scan
        ADDRESS(0x05 - (info->size_lib + info->rip + 4));

        if (loc_var.number == 0)
        {
            BYTE3(0x41, 0x8F, 0x06);
        }

        else if (loc_var.number > 0 && loc_var.number < 16)
        {
            BYTE4(0x41, 0x8F, 0x46, loc_var.number << 3);
        }

        else
        {
            BYTE3(0x41, 0x8F, 0x86);
            ADDRESS(loc_var.number << 3);
        }
    }
}

void Print_assign(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);

    struct Variable var = info->global_var.get_value(current_node->left->str);

    if (!Type_traits<struct Variable>::IsPoison(var))
    {
        Print_equation(tree, current_node->right, info, count_var, bytecode, reg);
        CLEAR_XMM0();

        if (var.number == 0)
        {
            BYTE3(0x41, 0x8F, 0x07);
        }

        else if (var.number > 0 && var.number < 16)
        {
            BYTE4(0x41, 0x8F, 0x47, var.number << 3);
        }

        else
        {
            BYTE3(0x41, 0x8F, 0x87);
            ADDRESS(var.number << 3);
        }
    }

    else 
    {
        struct Variable loc_var = info->locale_var.get_value(current_node->left->str);

        if (Type_traits<struct Variable>::IsPoison(loc_var))
        {
            tree->error = VAR_NOT_FOUND;
            TREE_ASSERT_OK(tree);
        }

        info->locale_var.remove(current_node->left->str);
        info->locale_var.set_value(current_node->left->str, {loc_var.number, 1});

        Print_equation(tree, current_node->right, info, count_var, bytecode, reg);
        CLEAR_XMM0();

        if (loc_var.number == 0)
        {
            BYTE3(0x41, 0x8F, 0x06);
        }

        else if (loc_var.number > 0 && loc_var.number < 16)
        {
            BYTE4(0x41, 0x8F, 0x46, loc_var.number << 3);
        }

        else
        {
            BYTE3(0x41, 0x8F, 0x86);
            ADDRESS(loc_var.number << 3);
        }
    }
}

void Print_arg(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;

    if (current_node->type == LR)
    {
        if (current_node->left != nullptr)  Print_arg(tree, current_node->left,  info, count_var, bytecode, reg);
        if (current_node->right != nullptr) Print_arg(tree, current_node->right, info, count_var, bytecode, reg);
    }

    else
    {  
        Print_equation(tree, current_node, info, count_var, bytecode, reg);
    }
}

void Print_if(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    size_t locale = info->count_label++;

    Print_equation(tree, current_node->left,  info, count_var, bytecode, reg);
    CLEAR_XMM0();
    

    BYTE1(0x59);                    // pop rcx
    BYTE4(0x48, 0x83, 0xF9, 0x00); // cmp rcx, 0
    BYTE2(0x0F, 0x84);             // je :if_end
    ADDRESS(info->label.get_value(locale) - (info->rip + 4));

    if (current_node->right->type == LR) Print_body(tree, current_node->right, info, count_var, bytecode, reg);
    else                                   Print_op(tree, current_node->right, info, count_var, bytecode, reg);
    
    info->label.set_value(locale, info->rip); // if_end:
}

void Print_while(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(info);

    size_t locale      = info->count_label;
    info->count_label += 2;
    
    info->label.set_value(locale, info->rip); // While_begin:

    Print_equation(tree, current_node->left, info, count_var, bytecode, reg);
    CLEAR_XMM0();

    BYTE1(0x59);                   // pop rcx
    BYTE4(0x48, 0x83, 0xF9, 0x00); // cmp rcx, 0
    BYTE2(0x0F, 0x84);             // je :While_end
    ADDRESS(info->label.get_value(locale + 1) - (info->rip + 4));

    if (current_node->right->type == LR) Print_body(tree, current_node->right, info, count_var, bytecode, reg);
    else                                   Print_op(tree, current_node->right, info, count_var, bytecode, reg);
    
    // jmp :While_begin
    BYTE1(0xE9);
    ADDRESS(info->label.get_value(locale) - (info->rip + 4));

    info->label.set_value(locale + 1, info->rip); // :While_end
}

void Print_call(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    BYTE2(0x41, 0x54); // push r12

    Print_arg(tree, current_node->left, info, count_var, bytecode, reg);

    if (count_var < 16)
    {
        BYTE4(0x49, 0x83, 0xC6, count_var << 3); // add r14, number
    }

    else
    {
        BYTE3(0x49, 0x81, 0xC6);
        ADDRESS(count_var << 3);
    }

    size_t shift_func = info->func.get_value(current_node->str);

    BYTE1(0xE8); // call :func
    ADDRESS(shift_func - (info->rip + 4));

    if (count_var < 16)
    {
        BYTE4(0x49, 0x83, 0xEE, count_var << 3); // sub r14, number
    }

    else
    {
        BYTE3(0x49, 0x81, 0xEE);
        ADDRESS(count_var << 3);
    }

    BYTE2(0x41, 0x5b); // pop  r11
    BYTE2(0x41, 0x5c); // pop  r12
    BYTE2(0x41, 0x53); // push r11
}

void Print_ret(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    Print_equation(tree, current_node->left, info, count_var, bytecode, reg);
    CLEAR_XMM0();

    BYTE2(0x41, 0x54); // push r12

    BYTE1(0xC3); // ret
}

void Print_equation(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode, struct Registers* reg)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);
    assert(reg);

    if (current_node == nullptr) return;
    
    switch (current_node->type)
    {
        case NUMBER:
        {
            unsigned long long int_value = *(unsigned long long*)&current_node->value;

            if (int_value < 0x80000000)
            {
                BYTE3(0x49, 0xc7, 0xc5); // mov r13, number
                ADDRESS(int_value);
            }            

            else
            {
                BYTE2(0x49, 0xBD);
                NUMBER(current_node->value)
            }

            BYTE2(0x41, 0x55); // push r13
            XMM0_RSP(-8);

            break;
        }
        
        case VAR:
        {
            struct Variable var = info->global_var.get_value(current_node->str);

            if (!Type_traits<struct Variable>::IsPoison(var))
            {
                if (var.number == 0)
                {
                    BYTE3(0x41, 0xFF, 0x37); // push [r15]
                    XMM0_RSP(-8);
                }

                else if (var.number > 0 && var.number < 16)
                {
                    BYTE4(0x41, 0xFF, 0x77, var.number << 3); // push [r15 + 8 * number] 

                    XMM0_RSP(-8);
                } 

                else
                {
                    BYTE3(0x41, 0xFF, 0xB7); // push [r15 + 8 * number] 
                    ADDRESS(var.number << 3);

                    XMM0_RSP(-8);
                }
            }

            else 
            {
                struct Variable loc_var = info->locale_var.get_value(current_node->str);

                if (Type_traits<struct Variable>::IsPoison(loc_var))
                {
                    tree->error = VAR_NOT_FOUND;
                    TREE_ASSERT_OK(tree);
                }

                if (loc_var.initialization == 0)
                {
                    *(int*)0 = 0;
                    // tree->error = NO_INITIALIZATION_VAR;
                    // TREE_ASSERT_OK(tree);
                }

                if (loc_var.number == 0)
                {
                    BYTE3(0x41, 0xFF, 0x36);

                    XMM0_RSP(-8);
                }

                else if (loc_var.number > 0 && loc_var.number < 16)
                {
                    BYTE4(0x41, 0xFF, 0x76, loc_var.number << 3);

                    XMM0_RSP(-8);
                } 

                else
                {
                    BYTE3(0x41, 0xFF, 0xB6);
                    ADDRESS(loc_var.number << 3);

                    XMM0_RSP(-8);
                }
            }
            
            break;
        }

        case OPERATION:
        {
            Print_equation(tree, current_node->left,  info, count_var, bytecode, reg);
            Print_equation(tree, current_node->right, info, count_var, bytecode, reg);

            switch ((int)current_node->value)
            {
                case 41: //ADD
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);                // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]

                    BYTE4(0xF2, 0x0F, 0x58, 0xC1);                      // addsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);                // add rsp, 8

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);                // movsd [rsp], xmm0

                    break;
                }
                case 42: // SUB
                {
                    if (current_node->left != nullptr)
                    {
                        BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);                // movsd xmm1, [rsp]

                        XMM0_RSP(8);
                        IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                        BYTE4(0xF2, 0x0F, 0x5C, 0xC1);                      // subsd xmm0, xmm1
                        BYTE4(0x48, 0x83, 0xC4, 0x08);                // add rsp, 8

                        SAVE_XMM0();
                        BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);                // movsd [rsp], xmm0
                    }

                    else
                    {
                        BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                        BYTE4(0x66, 0x0F, 0x57, 0xC0);             // xorpd xmm0, xmm0
                        BYTE4(0xF2, 0x0F, 0x5C, 0xC1);             // subsd xmm0, xmm1

                        //SAVE_XMM0(5);
                        BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0
                    }
                   

                    break;
                }
                case 43: // MUL
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);                // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE4(0xF2, 0x0F, 0x59, 0xC1);                      // mulsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);                      // add rsp, 8

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);                // movsd [rsp], xmm0

                    break;
                }
                case 44: // DIV
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);                // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE4(0xF2, 0x0F, 0x5E, 0xC1);                      // divsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);                      // add rsp, 8

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);                // movsd [rsp], xmm0

                    break;
                }
                case 45: // POW
                {
                    break;
                }   
                case 46: // BAA
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x06);       // cmpsd xmm0, xmm1, 6 (xmm0 > xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 47: // BAB
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x01);       // cmpsd xmm0, xmm1, 1 (xmm0 < xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 48: // BAE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x00);       // cmpsd xmm0, xmm1, 0 (xmm0 == xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 49: // BAAE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x05);       // cmpsd xmm0, xmm1, 5 (xmm0 > xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 50: // BABE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x02);       // cmpsd xmm0, xmm1, 2 (xmm0 <= xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 51: // BANE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]

                    XMM0_RSP(8);
                    IF_XMM0(BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08)); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x4);       // cmpsd xmm0, xmm1, 4 (xmm0 != xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);      // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                
                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }

            break;
        }

        case FUNC:
        {
            if (current_node->value >= 65 && current_node->value <= 79)
            {
                Print_arg(tree, current_node->left, info, count_var, bytecode, reg);
            }

            switch ((int)current_node->value)
            {
                case 65: /* LN     */ break;
                case 66: /* SIN    */ break;
                case 67: /* COS    */ break;
                case 68: /* TG     */ break;
                case 69: /* CTG    */ break;
                case 70: /* ARCSIN */ break;
                case 71: /* ARCCOS */ break;
                case 72: /* ARCTG  */ break;
                case 73: /* ARCCTG */ break;
                case 74: /* SH     */ break;
                case 75: /* CH     */ break;
                case 76: /* TH     */ break;
                case 77: /* CTH    */ break;
                case 78: /* SQRT   */ 
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);  // movsd xmm1, [rsp]
                    BYTE4(0xF2, 0x0F, 0x51, 0xC1);        // sqrtsd xmm0, xmm1

                    SAVE_XMM0();
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);  // movsd [rsp], xmm0
                    break;
                }
                case 79: /* EXP    */ break;
                
                default:
                    Print_call(tree, current_node, info, count_var, bytecode, reg);
                    break;
            }

            break;
        }

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }
}

//=============================================================================================================

int Find_main(struct Tree* tree)
{
    Tree_null_check(tree);

    struct Node* current_node = tree->root;

    do
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == FUNC && current_node->left->value == 1) return 1;
            else current_node = current_node->right;
        }
        else current_node = current_node->right;
    } while (current_node->type == DECLARATE && current_node->right != nullptr);

    if (current_node == nullptr) return 0;

    if (current_node->type == FUNC && current_node->value == 1) return 1;
    else return 0;
}

void Find_locale_var(struct Tree* tree, struct Node* current_node, struct Code* info)
{
    Tree_null_check(tree);
    assert(info);

    if (current_node == nullptr) return;
    
    if (current_node->type == VAR)
    {
        if (Type_traits<struct Variable>::IsPoison(info->global_var.get_value(current_node->str)) && \
            Type_traits<struct Variable>::IsPoison(info->locale_var.get_value(current_node->str)))
        {
            struct Variable new_var = {info->locale_var.get_count(), 0};
            info->locale_var.set_value(current_node->str, new_var);
        }
    }

    if (current_node->left  != nullptr) Find_locale_var(tree, current_node->left,  info);
    if (current_node->right != nullptr) Find_locale_var(tree, current_node->right, info);
}
