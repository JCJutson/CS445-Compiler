#
# Generic Bison Template Makefile
#    ...by Dr. J
#
# Replace the macro definition below with this semester's application.
APP=c-
CCOMPILER=g++
PARSER=parsercpp
SCANTYPE=scanType.h treenode.h symtab.h
CFLAGS=-c -g -std=c++0x -Wno-write-strings
SOURCES=$(PARSER).l $(PARSER).y  main.cpp $(SCANTYPE) symtab.h symtab.cpp treenode.cpp printtree.cpp semantic.cpp yyerror.cpp yyerror.h code.cpp code.h codegen.h codegen.cpp
OBJECTS=main.o lex.yy.o $(PARSER).tab.o symtab.o treenode.o printtree.o semantic.o yyerror.o code.o codegen.o
ASSIGNMENT="CS445 F16 Assignment 7"
TAR=Ass7.tar
STUDENT=jutson

$(APP): $(OBJECTS)
	$(CCOMPILER) -g -o $(APP) $(OBJECTS)

main.o: main.cpp $(PARSER).tab.h $(SCANTYPE) yyerror.h
	$(CCOMPILER) $(CFLAGS) main.cpp

yyerror.o: yyerror.cpp yyerror.h
	$(CCOMPILER) $(CFLAGS) yyerror.cpp

lex.yy.o: lex.yy.c
	$(CCOMPILER) $(CFLAGS) lex.yy.c

lex.yy.c: $(PARSER).l $(PARSER).tab.h
	flex $(PARSER).l

$(PARSER).tab.o: $(PARSER).tab.c
	$(CCOMPILER) $(CFLAGS) $(PARSER).tab.c

$(PARSER).tab.h $(PARSER).tab.c: $(PARSER).y treenode.h scanType.h symtab.h yyerror.h
	bison -v -t -d $(PARSER).y

symtab.o: symtab.h symtab.cpp
	$(CCOMPILER) $(CFLAGS) symtab.cpp

treenode.o: treenode.h treenode.cpp $(PARSER).tab.h printtree.cpp
	$(CCOMPILER) $(CFLAGS) -c treenode.cpp

printtree.o: printtree.cpp treenode.h $(PARSER).tab.h
	$(CCOMPILER) $(CFLAGS) -c printtree.cpp

semantic.o: semantic.cpp treenode.h $(PARSER).tab.h symtab.h
	$(CCOMPILER) $(CFLAGS) semantic.cpp

code.o: code.cpp code.h
	$(CCOMPILER) $(CFLAGS) code.cpp

codegen.o: codegen.cpp codegen.h code.o
	$(CCOMPILER) $(CFLAGS) codegen.cpp

tar: makefile $(SOURCES)
	tar -cvf $(TAR) makefile $(SOURCES)

turnin: tar
	curl -F student=$(STUDENT) -F assignment=$(ASSIGNMENT) -F "submittedfile=@$(TAR)" "http://ec2-52-89-93-46.us-west-2.compute.amazonaws.com/cgi-bin/fileCapture.py"

overkill: $(APP)
	./c- -P testData/overkill.c- > overkill.out
	sdiff -w 168 overkill.out testData/overkill.Pout > overkilldiff
