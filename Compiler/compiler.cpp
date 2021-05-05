#include "compiler.h"

void Create_elf(struct Tree* tree, FILE* file)
{
    struct Code info = {0, 0, 0, 0, 0, 0, 0, {MAX_SIZE_HASH_TABLE_FUNC,   hash_crc},     \
                                             {MAX_SIZE_HASH_TABLE_VARS,   hash_crc},     \
                                             {MAX_SIZE_HASH_TABLE_VARS,   hash_crc},     \
                                             {MAX_SIZE_HASH_TABLE_LABELS, hash_crc}, 0};

    char* lib      = Include_lib(&info);
    char* bytecode = (char*) calloc(tree->size * MAX_SIZE_CMD, sizeof(char));

    if (!Find_main(tree)) 
    {
        tree->error = NO_MAIN_DECLARATION;
        TREE_ASSERT_OK(tree);
    }

    Compile_pass(tree, &info, bytecode);
    Compile_pass(tree, &info, bytecode);

    info.size_headers += Add_ELF_header(file);
    info.size_headers += Add_program_header(file);

    Add_program_code_header(file, &info);
    Add_program_data_header(file, &info);

    Write_content_elf(file, &info, lib, bytecode);
}

char* Include_lib(struct Code* info)
{
    assert(info);

    FILE* lib = fopen("libr/IOlib1", "rb");

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
    NUMBER(info->offset_data * 0x1000 + 0x401000 + info->count_var << 3);

    BYTE3(0x49, 0xC7, 0xC7); // mov r15, data
    NUMBER(info->offset_data * 0x1000 + 0x401000);

    Initialization_global_vars(tree, info, bytecode);
    Begin_translation(         tree, info, bytecode);
}

void Initialization_global_vars(struct Tree* tree, struct Code* info, char* bytecode)
{
    assert(tree);
    assert(info);
    assert(bytecode);

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
                    var.number         = info->global_var.get_size();
                    var.initialization = 1;

                    info->global_var.set_value(new_var->str, var);
                    ++info->count_var;
                    info->size_data += 8;
                }

                Print_equation(tree, current_node->left->right, info, 0, bytecode);

                if (var.number == 0)
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
                var.number         = info->global_var.get_size();
                var.initialization = 1;

                info->global_var.set_value(new_var->str, var);
                ++info->count_var;
                info->size_data += 8;
            }

            Print_equation(tree, current_node->right, info, 0, bytecode);

            if (var.number == 0)
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

void Begin_translation(struct Tree* tree, struct Code* info, char* bytecode)
{
    assert(tree);
    assert(info);
    assert(bytecode);

    info->func.get_value("main");

    sprintf(bytecode, "push %lu\n"
                  "pop rfx\n"
                  "call :main\n"
                  "hlt\n\n\n", ram->global_var->size);
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

void Print_equation(struct Tree* tree, struct Node* current_node, struct Code* info, size_t count_var, char* bytecode)
{
    Tree_null_check(tree);
    assert(info);
    assert(bytecode);

    if (current_node == nullptr) return;
    
    switch (current_node->type)
    {
        case NUMBER:
        {
            BYTE3(0x49, 0xC7, 0xC5);
            NUMBER(current_node->value);

            break;
        }
        
        case VAR:
        {
            struct Variable var = info->global_var.get_value(current_node->str);

            if (!Type_traits<struct Variable>::IsPoison(var))
            {
                if (var.number == 0)
                {
                    BYTE3(0x41, 0xFF, 0x37);
                }

                else if (var.number > 0 && var.number < 16)
                {
                    BYTE4(0x41, 0xFF, 0x77, var.number << 3);
                } 

                else
                {
                    BYTE3(0x41, 0xFF, 0xB7);
                    ADDRESS(var.number << 3);
                }
            }

            else 
            {
                struct Variable var = info->locale_var.get_value(current_node->str);

                if (!Type_traits<struct Variable>::IsPoison(var))
                {
                    tree->error = VAR_NOT_FOUND;
                    TREE_ASSERT_OK(tree);
                }

                if (var.initialization == 0)
                {
                    *(int*)0 = 0;
                    // tree->error = NO_INITIALIZATION_VAR;
                    // TREE_ASSERT_OK(tree);
                }

                if (var.number == 0)
                {
                    BYTE3(0x41, 0xFF, 0x36);
                }

                else if (var.number > 0 && var.number < 16)
                {
                    BYTE4(0x41, 0xFF, 0x76, var.number << 3);
                } 

                else
                {
                    BYTE3(0x41, 0xFF, 0xB6);
                    ADDRESS(var.number << 3);
                }
            }
            
            break;
        }

        case OPERATION:
        {
            Print_equation(tree, current_node->left,  info, count_var, bytecode);
            Print_equation(tree, current_node->right, info, count_var, bytecode);

            switch ((int)current_node->value)
            {
                case 41: //ADD
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE4(0xF2, 0x0F, 0x58, 0xC1);             // addsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 42: // SUB
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE4(0xF2, 0x0F, 0x5C, 0xC1);             // subsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 43: // MUL
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE4(0xF2, 0x0F, 0x59, 0xC1);             // mulsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 44: // DIV
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE4(0xF2, 0x0F, 0x5E, 0xC1);             // divsd xmm0, xmm1
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 45: // POW
                {
                    break;
                }   
                case 46: // BAA
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x06);       // cmpsd xmm0, xmm1, 6 (xmm0 > xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 47: // BAB
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x01);       // cmpsd xmm0, xmm1, 1 (xmm0 < xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 48: // BAE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x00);       // cmpsd xmm0, xmm1, 0 (xmm0 == xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 49: // BAAE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x05);       // cmpsd xmm0, xmm1, 5 (xmm0 > xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 50: // BABE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x02);       // cmpsd xmm0, xmm1, 2 (xmm0 <= xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);       // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);       // movsd [rsp], xmm0

                    break;
                }
                case 51: // BANE
                {
                    BYTE5(0xF2, 0x0F, 0x10, 0x0C, 0x24);       // movsd xmm1, [rsp]
                    BYTE6(0xF2, 0x0F, 0x10, 0x44, 0x24, 0x08); // movsd xmm0, [rsp + 8]
                    BYTE5(0xF2, 0x0F, 0xC2, 0xC1, 0x4);       // cmpsd xmm0, xmm1, 4 (xmm0 != xmm1) 
                    BYTE4(0x48, 0x83, 0xC4, 0x08);             // add rsp, 8
                    BYTE3(0x49, 0xC7, 0xC5);                   // mov r13, 1
                    ADDRESS(0x1);
                    BYTE5(0xF2, 0x49, 0x0F, 0x2A, 0xCD);      // cvtsi2sd xmm1, r13
                    BYTE4(0x66, 0x0F, 0x54, 0xC1);             // andpd xmm0, xmm1
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
                Print_arg(tree, current_node->left, info, count_var, bytecode);
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
                    BYTE5(0xF2, 0x0F, 0x11, 0x04, 0x24);  // movsd [rsp], xmm0
                    break;
                }
                case 79: /* EXP    */ break;
                
                default:
                    Print_call(tree, current_node, info, count_var, bytecode);
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

    while (current_node->type == DECLARATE && current_node->right != nullptr)
    {
        if (current_node->left != nullptr)
        {
            if (current_node->left->type == FUNC && current_node->left->value == 1) return 1;
            else current_node = current_node->right;
        }
        else current_node = current_node->right;
    }

    if (current_node == nullptr) return 0;

    if (current_node->type == FUNC && current_node->value == 1) return 1;
    else return 0;
}

//=============================================================================================================

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

char* Get_word(char* begin)
{
    char* current_symbol = begin;

    while (!isspace(*current_symbol) && *current_symbol != '\0' && *current_symbol != '\n') ++current_symbol;

    return strndup(begin, current_symbol - begin); 
}

size_t Skip_separator(char** string)
{
    assert(string != nullptr);

    size_t count = 0;

    while ((isspace((int)(unsigned char)**string) || **string == '\n') && **string != '\0')
    {
        ++(*string);
        ++count;
    }

    return count; 
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

//=============================================================================================================