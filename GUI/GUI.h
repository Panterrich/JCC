#ifndef GUI_H 
#define GUI_H

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
#include "../libr/LibEnd.h"

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

struct Node* GetProg(struct Tree* tree, struct Stack* nodes, const char* name_program);

void Free_nodes(struct Stack* nodes);

void Tree_print(struct Tree* tree);

void Tree_processing(struct Tree* tree);

void Create_elf(struct Tree* tree, FILE* file);

void Tree_create(struct Tree* tree, struct Text* text, const char* name_program);

int Compile(const char* name_program);

int Create_window(sf:: Music* music, sf:: Music* joke_music);

void Set_text(sf::Text* text, const sf::Font& font, const sf::Color color, size_t font_size, float x, float y);

void Set_text(sf::Text* text, const sf::Font& font, const sf::Color color, size_t font_size, float x, float y, const char* str);

#endif