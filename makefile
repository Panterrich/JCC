THIS_FILE := $(lastword $(MAKEFILE_LIST))

.PHONY: all
.PHONY: clean_tex
.PHONY: clean_all_image

all: 
	@echo $@  # print target name
	@$(MAKE) -f $(THIS_FILE) fe-rm
	@$(MAKE) -f $(THIS_FILE) fe-hg
	@$(MAKE) -f $(THIS_FILE) rfe-rm
	@$(MAKE) -f $(THIS_FILE) rfe-hg
	@$(MAKE) -f $(THIS_FILE) me
	@$(MAKE) -f $(THIS_FILE) compiler
	@$(MAKE) -f $(THIS_FILE) JCC

JCC: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp libr/hashtable/Hash_func.cpp libr/LibEnd.cpp Front-end/Frontend.cpp GUI/GUI.cpp Middle-end/Middleend.cpp Compiler/compiler.cpp GUI/main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp GUI/GUI.cpp Middle-end/Middleend.cpp Compiler/compiler.cpp GUI/main.cpp libr/LibEnd.cpp \
	libr/Differentiator/Differentiator.cpp libr/hashtable/Hash_func.cpp -msse4.2 -mavx2 -march=native -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o build/JCC


run_hg:
	./build/fe-hg $(NAME).txt
	./build/me    $(NAME).me
	./build/compl $(NAME).jcc

run_rm:
	./build/fe-rm $(NAME).txt
	./build/me    $(NAME).me
	./build/compl $(NAME).jcc

clean:
	rm -f $(NAME).me
	rm -f $(NAME).jcc
	rm -f $(NAME).elf

clean_build:
	rm -f build/fe-rm
	rm -f build/fe-hg
	rm -f build/rfe-rm
	rm -f build/rfe-hg
	rm -f build/me
	rm -f build/compl

fe-hg: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/LibEnd.cpp Front-end/Frontend.cpp Front-end/main.cpp 
	@echo $@  # print target name
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/LibEnd.cpp Front-end/Frontend.cpp Front-end/main.cpp -o build/fe-hg -D HG

fe-rm: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/LibEnd.cpp Front-end/Frontend.cpp Front-end/main.cpp
	@echo $@  # print target name
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/LibEnd.cpp Front-end/Frontend.cpp Front-end/main.cpp -o build/fe-rm

rfe-hg: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp
	@echo $@  # print target name
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o build/rfe-hg -D HG

rfe-rm: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp
	@echo $@  # print target name
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o build/rfe-rm

me: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp libr/LibEnd.cpp Middle-end/Middleend.cpp Middle-end/main.cpp
	@echo $@  # print target name
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp libr/LibEnd.cpp Middle-end/Middleend.cpp Middle-end/main.cpp -o build/me

compiler: Compiler/compiler.cpp Compiler/main.cpp libr/hashtable/Hash_func.cpp libr/LibEnd.cpp
	@echo $@  # print target name
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/LibEnd.cpp Compiler/compiler.cpp Compiler/main.cpp libr/hashtable/Hash_func.cpp -msse4.2 -mavx2 -march=native -o build/compl


clean_all_image: 
	@echo $@  # print target name
	@$(MAKE) -f $(THIS_FILE) clean_tex
	rm -f "images/tree_graph.dot"
	rm -f images/base_*

clean_tex:
	rm -f "images/equation.aux"
	rm -f "images/equation.log"
	rm -f "images/equation.out"
	rm -f "images/equation.tex"
	rm -f "images/tree_graph.dot"
	rm -f "images/end.log"
	rm -f "images/end.tex.bak"
	rm -f "images/equation.tex.bak"
	rm -f "images/preambule_article.tex.bak"
	rm -f "images/title.tex.bak"
	rm -f "images/title.log"
