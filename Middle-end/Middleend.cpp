#include "Middleend.h"

struct Node* Tree_search_node(struct Tree* tree, struct Node* current_node, int type, int number)
{
    Tree_null_check(tree);
    
    if (current_node == nullptr) return nullptr;

    struct Node* left  = Tree_search_node(tree, current_node->left, type, number);
    struct Node* right = Tree_search_node(tree, current_node->right, type, number);

    if (left == nullptr && right == nullptr)
    {
        if (current_node->type == type && current_node->value == number)
        {
            return current_node;
        }

        else
        {
            return nullptr;
        }
    }

    if (left  != nullptr) return left;
    if (right != nullptr) return right;

    return nullptr;
}

void Tree_processing(struct Tree* tree)
{
    Tree_null_check(tree);

    struct Node* current_node = nullptr;

    while ((current_node = Tree_search_node(tree, tree->root, OPERATION, 52)) != nullptr)
    {   
        current_node->right = Derivative(tree, current_node->right, current_node->left->str, Hash(current_node->left->str));
        Re_linking_subtree(tree, current_node, current_node->right, current_node->left);
    }
     
    Optimization(tree);

    #ifdef ALLDIF
    My_derivative(tree, tree->root);
    Optimization(tree);
    #endif
}

struct Node* My_derivative(struct Tree* tree, struct Node* current_node)
{
    Tree_null_check(tree);
    
    if (current_node == nullptr) return nullptr;
   
    #include "My_derivative_DSL.h"

    switch (TYPE)
    {
        case DECLARATE:
        {
            My_derivative(tree, current_node->left);
            My_derivative(tree, current_node->right);
            break;
        }

        case LR: 
        {
            LNODE = dL;
            RNODE = dR;

            break;
        }

        case NUMBER: current_node->value = 0; break;
        case VAR: 
        {
            free(current_node->str);

            current_node->type = NUMBER;
            current_node->str = nullptr;
            current_node->value = 1;

            break;
        }

        case OPERATION: 
        {
            if (current_node->value == KEY_ASSIGN)
            {
                if (current_node->prev != nullptr)
                {
                    if (current_node->prev->type == DECLARATE)
                    {
                        break;
                    }
                }
            }

            switch (OP)
            {
                case KEY_RETURN:
                    My_derivative(tree, current_node->left);
                    break;

                case KEY_ASSIGN:
                    My_derivative(tree, current_node->right);
                    break;

                case KEY_IF:
                case KEY_WHILE:
                    My_derivative(tree, current_node->left);
                    My_derivative(tree, current_node->right);
                    break;

                #define DEF_OP(operator, op, number, code) case op: code; break;
                #define DEF_FUNC(func, FUNC, hash, number, code)
                
                #include "../libr/Differentiator/commands.h"

                #undef DEF_OP
                #undef DEF_FUNC

                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            
            break;
        }
        
        case FUNC:
        {
            if (current_node->prev != nullptr)
            {
                if (current_node->prev->type == DECLARATE)
                {
                    My_derivative(tree, current_node->right);
                
                    break;
                } 
            }

            switch (OP)
            {
                case KEY_PRINT: My_derivative(tree, current_node->left); break;
                case KEY_SCAN: break;

                #define DEF_OP(operator, OP, number, code)
                #define DEF_FUNC(func, value, hash, number, code) case value: code; break;
                
                #include "../libr/Differentiator/commands.h"

                #undef DEF_OP
                #undef DEF_FUNC

                default:
                    My_derivative(tree, current_node->left);
                    break;
            }

            break;        
        }

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }

    tree->size = Size_subtree(tree, tree->root);
    
    return current_node;
}