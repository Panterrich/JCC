#ifndef MIDDLEENDH
#define MIDDLEENDH

//=============================================================================================================
//#define ALLDIF
//=============================================================================================================

#include "../libr/Onegin/Onegin.h"
#include "../libr/Tree/Tree.h"
#include "../libr/Stack/Guard.h"
#include "../libr/Differentiator/Differentiator.h"
#include "../libr/LibEnd.h"

//=============================================================================================================

struct Node* Tree_search_node(struct Tree* tree, struct Node* current_node, int type, int number);

void Tree_processing(struct Tree* tree);

void All_differentiation(struct Tree* tree, struct Node* current_node);

struct Node* My_derivative(struct Tree* tree, struct Node* current_node);

#endif