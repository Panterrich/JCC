#ifndef FRONTENDH
#define FRONTENDH

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"
#include "../libr/LibEnd.h"

//================================================================================================================

struct Program
{
    char* buffer; 
    char* current_symbol;
};

//================================================================================================================

const size_t INITIAL_CAPACITY = 20;

//================================================================================================================

void Program_create(FILE* file, struct Program* program);

void Tokenizer(struct Program* program, struct Stack* nodes);

void Program_destruct(struct Program* program);

void Free_nodes(struct Stack* nodes);

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

//=============================================================================================================

const char* Get_path(int code);

#endif