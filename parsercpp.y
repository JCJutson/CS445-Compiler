%{
	#include"treenode.h"
	#define YYDEBUG 1
	#include<stdio.h>
	#include<iostream>
	#include"scanType.h"
	#include"symtab.h"
	#include"yyerror.h"
	
	using namespace std;
	
	extern FILE *yyin;
	extern char *yytext;
	extern char *yyfilename;
	extern int lineNum;
	extern int numErrors;
	extern int numWarnings;
	extern int yylex();
	extern SymTab recordSymTab;
	
	TreeNode* treeRoot = NULL;
	
%}

%error-verbose

%union {
  TokenData* tokenData;
  TreeNode* treeNode;
  DeclKind declKind;
}

%token <tokenData> NUMCONST  
%token <tokenData> CHARCONST
%token <tokenData> BOOLCONST
%token <tokenData> AND
%token <tokenData> OR
%token <tokenData> NOT
%token <tokenData> NOTEQ
%token <tokenData> MULASS
%token <tokenData> INC
%token <tokenData> ADDASS
%token <tokenData> DEC
%token <tokenData> SUBASS
%token <tokenData> DIVASS
%token <tokenData> LESSEQ
%token <tokenData> EQ
%token <tokenData> GRTEQ
%token <tokenData> ID 
%token <tokenData> RECORD
%token <tokenData> INT
%token <tokenData> BOOL
%token <tokenData> CHAR
%token <tokenData> STATIC
%token <tokenData> IF
%token <tokenData> WHILE
%token <tokenData> BREAK
%token <tokenData> ELSE
%token <tokenData> RETURN
%token <tokenData> RECTYPE

%type <treeNode> declarationList
%type <treeNode> declaration
%type <treeNode> varDeclaration
%type <treeNode> funDeclaration
%type <treeNode> recDeclaration
%type <treeNode> localDeclarations
%type <treeNode> scopedVarDeclaration
%type <treeNode> varDecList
%type <treeNode> varDecInitialize
%type <treeNode> varDecId
%type <treeNode> scopedTypeSpecificer
%type <treeNode> simpleExpression
%type <treeNode> unaryExpression
%type <treeNode> mutable
%type <treeNode> immutable
%type <treeNode> constant
%type <treeNode> factor
%type <treeNode> argList
%type <treeNode> args
%type <treeNode> call
%type <treeNode> relExpression
%type <treeNode> relop
%type <treeNode> sumop
%type <treeNode> term
%type <treeNode> mulop
%type <treeNode> expression
%type <treeNode> assignop
%type <treeNode> sumExpression
%type <treeNode> unaryop
%type <treeNode> params
%type <treeNode> paramId
%type <treeNode> paramIdList
%type <treeNode> paramTypeList
%type <treeNode> paramList
%type <treeNode> statement
%type <treeNode> matchedStatement
%type <treeNode> unmatchedStatement
%type <treeNode> matchedIterationStatement
%type <treeNode> matchedSelectionStatement
%type <treeNode> unmatchedIterationStatement
%type <treeNode> unmatchedSelectionStatement
%type <treeNode> expressionStatement
%type <treeNode> compoundStatement
%type <treeNode> returnStatement
%type <treeNode> breakStatement
%type <treeNode> andExpression
%type <treeNode> unaryRelExpression
%type <treeNode> statementList
%type <declKind> typeSpecifier
%type <declKind> returnTypeSpecifier
%type <tokenData> '+'
%type <tokenData> '-'
%type <tokenData> '*'
%type <tokenData> '/'
%type <tokenData> '?'
%type <tokenData> '<'
%type <tokenData> '>'
%type <tokenData> '%'
%type <tokenData> '{'
%type <tokenData> '}'
%type <tokenData> '('
%type <tokenData> ')'
%type <tokenData> ';'
%type <tokenData> '='
%type <tokenData> '['
%type <tokenData> ']'
%type <tokenData> '.'

%%

program
:declarationList {treeRoot = $1;}
;

declarationList
: declarationList declaration   {$1->appendSibling($2);
								 $$ = $1;}
| declaration				    {$$ = $1;}
;

declaration
: varDeclaration	{$$ = $1;}
| funDeclaration	{$$ = $1;}
| recDeclaration	{$$ = $1;}
| error				{$$ = NULL;}
;

recDeclaration
: RECORD ID '{' localDeclarations '}' { recordSymTab.insert($2->input, $2->input);
										$$ = new TreeNode(RECDECLK, $2);
										$$->child.push_back($4);}
;

varDeclaration
: typeSpecifier varDecList ';'		{ //get DeclKind from $1
										DeclKind dk = $1;
										
										//set each sibling in $2 to that type
										TreeNode* nodePtr = $2;
										while(nodePtr != NULL){
											nodePtr->kind.decl = dk;
											nodePtr = nodePtr->sibling;
										}
										$$ = $2;
										yyerrok;
									}
| error varDecList ';'				{$$ = NULL;}
| typeSpecifier error ';'			{$$ = NULL;
									 yyerrok;}
;

scopedVarDeclaration
: scopedTypeSpecificer varDecList ';' { //get DeclKind and isStatic from $1
										DeclKind dk = $1->kind.decl;
										bool isStatic = $1->isStatic;
										
										//set each sibling in $2 to that type
										TreeNode* nodePtr = $2;
										while(nodePtr != NULL){
											nodePtr->kind.decl = dk;
											nodePtr->isStatic = isStatic;
											nodePtr = nodePtr->sibling;
										}
										$$ = $2;
										yyerrok;
									  }
| error varDecList ';'				{$$ = NULL;
									 yyerrok;}
| typeSpecifier error ';'			{$$ = NULL;
									 yyerrok;}
;

varDecList
: varDecList ',' varDecInitialize { $1->appendSibling($3);
									$$ = $1; 
									yyerrok; 
								  }
| varDecList ',' error			  { $$ = $1;}
| varDecInitialize				  { $$ = $1; }
| error							  { $$ = NULL; }
;

varDecInitialize
: varDecId						  { $$ = $1; }
| varDecId ':' simpleExpression   { $$ = $1;
									$1->child.push_back($3); }
| error ':' simpleExpression 	  { $$ = NULL;
									yyerrok;}
| varDecId ':' error			  { $$ = $1; }
;

varDecId
: ID					{$$ = new TreeNode(DECLK, $1);}
| ID '[' NUMCONST ']'	{$$ = new TreeNode(DECLK, $1);
						 $$->isArray = true;
						 $$->size = 1 + $3->value;}
| ID '[' error			{$$ = NULL;}
| error ']'				{$$= NULL;
						 yyerrok;}
;

scopedTypeSpecificer
: STATIC typeSpecifier{ $$ = new TreeNode;
						$$->isStatic = true;
						$$->kind.decl = $2;}
| typeSpecifier		  {	$$ = new TreeNode;
						$$->isStatic = false;
						$$->kind.decl = $1;}
;

typeSpecifier
: returnTypeSpecifier 	{$$ = $1;}
| RECTYPE			  	{$$ = RECORDK;}
;

returnTypeSpecifier
: INT					{$$ = INTK;}
| BOOL					{$$ = BOOLK;}
| CHAR					{$$ = CHARK;}
;

funDeclaration
: typeSpecifier ID '(' params ')' statement {$$ = new TreeNode(FUNDECLK, $2);
											 $$->kind.decl = $1;
											 $$->child.push_back($4);
											 if($6 != NULL)
												$6->isStatementUnderFunction = true;
											 $$->child.push_back($6);}
| ID '(' params ')' statement				{$$ = new TreeNode(FUNDECLK, $1);
											 $$->kind.decl = VOIDK;
											 $$->child.push_back($3);
											 if($5 != NULL)
												$5->isStatementUnderFunction = true;
											 $$->child.push_back($5);}
| typeSpecifier error						{$$ = NULL;}
| typeSpecifier ID '(' error				{$$ = NULL;}
| typeSpecifier ID '(' params ')' error		{$$ = NULL;}
| ID '(' error								{$$ = NULL;}
| ID '(' params ')' error					{$$ = NULL;}
;

params
: paramList	{$$ = $1;}
| 			{$$ = NULL;}
;

paramList
: paramList ';' paramTypeList	{$1->appendSibling($3);
								 $$ = $1;
								 yyerrok;}
| paramTypeList					{$$ = $1;}
| paramList ';' error		{$$ = $1;}
| error							{$$ = NULL;}
;

paramTypeList
: typeSpecifier paramIdList			{ //get DeclKind from $1
										DeclKind dk = $1;
										
										//set each sibling in $2 to that type
										TreeNode* nodePtr = $2;
										while(nodePtr != NULL){
											nodePtr->kind.decl = dk;
											nodePtr = nodePtr->sibling;
										}
										$$ = $2;
									}
| typeSpecifier error				{$$ = NULL;}
;

paramIdList
: paramIdList ',' paramId		{$1->appendSibling($3);
								 $$ = $1;
								 yyerrok;}
| paramId						{$$ = $1;}
| paramIdList ',' error			{$$ = $1;}
| error							{$$ = NULL;}
;

paramId
: ID							{$$ = new TreeNode(PARAMK, $1);}
| ID '[' ']'					{$$ = new TreeNode(PARAMK, $1);
								 $$->isArray = true;}
| error ']'						{$$ = NULL;
								 yyerrok;}
;

statement
: matchedStatement			{$$ = $1;}
| unmatchedStatement		{$$ = $1;}
;

matchedStatement
: expressionStatement			{$$ = $1;}
| compoundStatement				{$$ = $1;}
| returnStatement				{$$ = $1;}
| breakStatement				{$$ = $1;}
| matchedSelectionStatement		{$$ = $1;}
| matchedIterationStatement		{$$ = $1;}
;

unmatchedStatement
: unmatchedSelectionStatement	{$$ = $1;}
| unmatchedIterationStatement	{$$ = $1;}
;

compoundStatement
: '{' localDeclarations statementList '}'	{$$ = new TreeNode(STMTK, $1);
											 $$->kind.stmt = COMPOUNDK;
											 $$->child.push_back($2);
											 $$->child.push_back($3);
											 yyerrok;
											 }
| '{' error statementList '}'				{$$ = NULL;
											 yyerrok;}
| '{' localDeclarations error '}'			{$$ = NULL;
											 yyerrok;}
;

localDeclarations
: localDeclarations scopedVarDeclaration  { if($1 == NULL)
												$$ = $2;
											else{
												$1->appendSibling($2);
												$$ = $1;
											}}
|	{$$ = NULL;}
;

statementList
: statementList statement 	{if($1 == NULL){
								$$ = $2;
							 }
							 else{
								$$ = $1;
								$$->appendSibling($2);}}
|							{$$ = NULL;}
;

expressionStatement
: expression ';'		{$$ = $1;
						 yyerrok; }
| ';'					{$$ = NULL;
						 yyerrok; }
;

matchedSelectionStatement
: IF '(' simpleExpression ')' matchedStatement ELSE matchedStatement	{$$ = new TreeNode(STMTK, $1);
																		 $$->kind.stmt = SELECTIONK;
																		 $$->child.push_back($3);
																		 $$->child.push_back($5);
																		 $$->child.push_back($7);
																		}
| IF '(' error															{$$ = NULL;}
| IF error ')' matchedStatement ELSE matchedStatement					{$$ = NULL;
																		 yyerrok;}
;

unmatchedSelectionStatement
: IF '(' simpleExpression ')' statement									{$$ = new TreeNode(STMTK, $1);
																		 $$->kind.stmt = SELECTIONK;
																		 $$->child.push_back($3);
																		 $$->child.push_back($5);
																		}
| IF '(' simpleExpression ')' matchedStatement ELSE unmatchedStatement	{$$ = new TreeNode(STMTK, $1);
																		 $$->kind.stmt = SELECTIONK;
																		 $$->child.push_back($3);
																		 $$->child.push_back($5);
																		 $$->child.push_back($7);
																		}
| IF error																{$$ = NULL;}
| IF error ')' statement 												{$$ = NULL;
																		 yyerrok; }
| IF error ')' matchedStatement ELSE unmatchedStatement 				{$$ = NULL;
																		 yyerrok; }
;

matchedIterationStatement
: WHILE '(' simpleExpression ')' matchedStatement	{$$ = new TreeNode(STMTK, $1);
													 $$->kind.stmt = ITERATIONK;
													 $$->child.push_back($3);
													 $$->child.push_back($5);
													}
| WHILE error ')' matchedStatement					{$$ = NULL;
													 yyerrok;}
| WHILE '(' error ')' matchedStatement				{$$ = NULL;
													 yyerrok;}
| WHILE error										{$$ = NULL;}
| error												{$$ = NULL;}
;

unmatchedIterationStatement
: WHILE '(' simpleExpression ')' unmatchedStatement	{$$ = new TreeNode(STMTK, $1);
													 $$->kind.stmt = ITERATIONK;
													 $$->child.push_back($3);
													 $$->child.push_back($5);
													}
| WHILE error ')' unmatchedStatement				{$$ = NULL;
													 yyerrok; }
| WHILE '(' error ')' unmatchedStatement			{$$ = NULL; 
													 yyerrok; }
;

returnStatement
: RETURN ';'				{$$ = new TreeNode(STMTK, $1);
							 $$->kind.stmt = RETURNK;
							 yyerrok;
							}
| RETURN expression ';'		{$$ = new TreeNode(STMTK, $1);
							 $$->kind.stmt = RETURNK;
							 $$->child.push_back($2);
							 yyerrok;
							}
;

breakStatement
: BREAK ';'			{$$ = new TreeNode(STMTK, $1);
					 $$->kind.stmt = BREAKK;
					 yyerrok;
					}
; 

expression
: mutable assignop expression	{$$ = $2;
								$$->child.push_back($1);
								$$->child.push_back($3);
								}
| error assignop error		{$$ = NULL;}
| mutable INC				{$$ = new TreeNode(OPK, $2);
							 $$->kind.op = INCK;
							 $$->child.push_back($1);
							 yyerrok;
							}
| error INC					{$$ = NULL;
							 yyerrok;}
| mutable DEC				{$$ = new TreeNode(OPK, $2);
							 $$->kind.op = DECK;
							 $$->child.push_back($1);
							 yyerrok;
							}
| error DEC 				{$$ = NULL;
							 yyerrok; }
| simpleExpression			{$$ = $1;}
;

assignop
: '='	 {$$ = new TreeNode(OPK, $1);
		  $$->kind.op =ASSK;}
| ADDASS {$$ = new TreeNode(OPK, $1);
		  $$->kind.op =ADDASSK;}
| SUBASS {$$ = new TreeNode(OPK, $1);
		  $$->kind.op =SUBASSK;}
| MULASS {$$ = new TreeNode(OPK, $1);
		  $$->kind.op = MULASSK;}
| DIVASS {$$ = new TreeNode(OPK, $1);
		  $$->kind.op = DIVASSK;}
;

simpleExpression
: simpleExpression OR andExpression	{$$ = new TreeNode(OPK, $2);
									 $$->kind.op = ORK;
									 $$->child.push_back($1);
									 $$->child.push_back($3);
									}
| simpleExpression OR error 		{$$ = $1;}
| andExpression						{$$ = $1;}
;

andExpression
: andExpression AND unaryRelExpression	{$$ = new TreeNode(OPK, $2);
										 $$->kind.op = ANDK;
										 $$->child.push_back($1);
										 $$->child.push_back($3);
										}
| andExpression AND error 				{$$ = $1;}
| unaryRelExpression					{$$ = $1;}
;

unaryRelExpression
: NOT unaryRelExpression	{$$ = new TreeNode(OPK, $1);
							 $$->kind.op = NOTK;
							 $$->child.push_back($2);
							}
| NOT error					{$$ = NULL;}
| relExpression				{$$ = $1;}
;

relExpression
: sumExpression relop sumExpression	{$$ = $2;
									 $$->child.push_back($1);
									 $$->child.push_back($3);}
| sumExpression relop error			{$$ = $1;}
| error relop sumExpression 		{$$ = $3;
									 yyerrok;}
| sumExpression						{$$ = $1;}
;

relop
: LESSEQ	{$$ = new TreeNode(OPK, $1);
			 $$->kind.op = LESSEQK;}
| '<'		{$$ = new TreeNode(OPK, $1);
			 $$->kind.op = LESSK;}
| '>'		{$$ = new TreeNode(OPK, $1);
			 $$->kind.op = GRTK;}
| GRTEQ		{$$ = new TreeNode(OPK, $1);
			 $$->kind.op = GRTEQK;}
| EQ		{$$ = new TreeNode(OPK, $1);
			 $$->kind.op = EQK;}
| NOTEQ		{$$ = new TreeNode(OPK, $1);
			 $$->kind.op = NOTEQK;}
;

sumExpression
: sumExpression sumop term	{$$ = $2;
							 $$->child.push_back($1);
							 $$->child.push_back($3);}
| sumExpression sumop error {$$ = $1;
							 yyerrok;}
| term						{$$ = $1;}
;

sumop
: '+'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = ADDK;}
| '-'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = SUBK;}
;

term
: term mulop unaryExpression	{$$ = $2;
								 $$->child.push_back($1);
								 $$->child.push_back($3);}
| term mulop error				{$$ = $1;}
| unaryExpression				{$$ = $1;}
;

mulop
: '*'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = MULK;}
| '/'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = DIVK;}
| '%'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = MODK;}
;

unaryExpression
: unaryop unaryExpression	{$$ = $1;
							 $$->child.push_back($2);}
| unaryop error				{$$ = NULL;}
| factor					{$$ = $1;}
;

unaryop
: '-'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = NEGK;}
| '*'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = ARRSIZEK;}
| '?'	{$$ = new TreeNode(OPK, $1);
		 $$->kind.op = RANDK;}
;

factor
: immutable	{$$ = $1;}
| mutable	{$$ = $1;}
;

mutable
: ID							{$$ = new TreeNode(EXPK, $1);
								 $$->kind.exp = VARK;}
| mutable '[' expression ']'	{$$ = new TreeNode(OPK, $2);
								 $$->kind.op = ARRACCK;
								 $$->child.push_back($1);
								 $$->child.push_back($3);}
| mutable '.' ID				{$$ = new TreeNode(OPK, $2);
								 $$->kind.op = DOTK;
								 $$->child.push_back($1);
								 $$->child.push_back(new TreeNode(EXPK, $3));
								 $$->child[1]->kind.exp = VARK;}
;
 
immutable
: '(' expression ')'{$$ = $2;
					 yyerrok; }
| '(' error			{$$ = NULL;}
| error ')'			{$$ = NULL; 
					 yyerrok; }
| call				{$$ = $1;}
| constant			{$$ = $1;}
;

call
: ID '(' args ')'	{$$ = new TreeNode(CALLK, $1);
					 $$->child.push_back($3);}
| error '(' 		{$$ = NULL; 
					 yyerrok;}
;

args
: argList	{$$ = $1;}
|			{$$ = NULL;}
;

argList
: argList ',' expression	{$$ = $1;
							 $$->appendSibling($3);
							 yyerrok; }
| argList ',' error 		{$$ = NULL;}
| expression				{$$ = $1;}
;

constant
: NUMCONST	{$$ = new TreeNode(CONSTK, $1);
			 $$->kind.constant = NUMC;}
| CHARCONST {$$ = new TreeNode(CONSTK, $1);
			 $$->kind.constant = CHARC;}
| BOOLCONST {$$ = new TreeNode(CONSTK, $1);
			 $$->kind.constant = BOOLC;}
;

%%

