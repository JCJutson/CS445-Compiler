/*
 * This version, migrated from your last HW, might need further adaptation
 * in order to work with a Bison parser.
 */
#include "scanType.h"
#include "treenode.h"
#include "parsercpp.tab.h"
#include "yyerror.h"
#include "codegen.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern FILE *yyin;
extern char *yytext;
extern int yydebug;
extern int lineNum;
extern int numErrors;
extern int numWarnings;
extern int currGlobalLocation;
extern char *yyfilename;
extern TreeNode *treeRoot;

using namespace std;

string newFileName(string s);

int main(int argc, char *argv[])
{
	initErrorProcessing();
	
	int i,t,opt;
	bool printTree = false, printTypedTree = false;
	while ((opt = getopt(argc, argv, "dpP")) != -1){
	        switch(opt){
		case 'd':
		  yydebug = 1;
			break;
		case 'p':
			printTree = true;
			break;
		case 'P':
			printTypedTree = true;
			break;
		default:
		    abort();
		}
	}
	
	if (optind < argc)
	{
		yyin = fopen(argv[optind],"r");
		if (yyin == NULL) 
		{ 
			printf("ERROR(ARGLIST): source file \"%s\" could not be opened.\n", argv[optind]); 
			exit(-1); 
		}
		yyfilename = argv[optind];
	}
	
	i = yyparse();
	
	if(numErrors == 0){
		if(printTree)
			treeRoot->print(false);
		
		treeRoot = prependIOPrototypes(treeRoot);
		
		treeRoot->scopeAndType();
		
		if(printTypedTree){
			treeRoot->print(true);
		}
	}
	
	string codeFileName = newFileName(string(yyfilename));
	
	if(numErrors == 0){
		codeGen(treeRoot, codeFileName);
	}
	
	std::cout << "Number of warnings: " << numWarnings << std::endl;
	std::cout << "Number of errors: " << numErrors << std::endl;
	
	
	
	//treeRoot->symTab.print();
	/*
		if (i != 0) {
			printf("parse failed\n");
		}
		else {
			printf("no errors\n");
			if(treeRoot != NULL){
				treeRoot->print();
			}
		}
		*/
	
	
	return 0;
}

string newFileName(string s){
	string r = s;
	if (s.size() < 2 || s[s.size() - 2] != 'c' || s[s.size() - 1] != '-' ){
		r = r + ".tm";
	}
	else {
		r[r.size() - 2] = 't';
		r[r.size() - 1] = 'm';
	}
	
	return r;
}
