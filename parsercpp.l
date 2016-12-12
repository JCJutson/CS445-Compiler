%{
    #include"scanType.h"
	#include "treenode.h"
    #include"parsercpp.tab.h"
	#include "symtab.h"
    #define AVLLDEBUG
    int lineNum = 1;
	int numErrors = 0;
	int numWarnings = 0;
    char* yyfilename;
	
	
	// a simple default print routine 
	void ePrint(void *p)
	{
		printf("[0x%08x]", p);
	}
	
	SymTab recordSymTab(ePrint);
	  
	char unescape(char* s){
		if(s == NULL || strlen(s) == 0){
			return 0;
		}
		int i = 0;
		if(s[0] == '\''){
			i++;
		}
		if(s[i] != '\\'){
			return s[i];
		}
		i++;
		switch(s[i]){
		case 'n': return '\n';
		case '0': return '\0';
		default: return s[i];
		}
	}
	  
	int setTokenData(int tokenClass){
        yylval.tokenData = new TokenData;
		yylval.tokenData->tokenClass = tokenClass;
		yylval.tokenData->lineNum = lineNum;
		yylval.tokenData->input = strdup(yytext);
                char* str;
		switch(tokenClass){
			case BOOLCONST:	
				if(strcmp(yytext, "true") == 0){
					yylval.tokenData->value = 1;
				}
				else {
					yylval.tokenData->value = 0;
				}
				yylval.tokenData->tokenName = "BOOLCONST";
				break;
			case NUMCONST:
				yylval.tokenData->value = atoi(yytext);
				yylval.tokenData->tokenName = "NUMCONST";
				break;
			case CHARCONST:
				yylval.tokenData->value = unescape(yytext); 
				yylval.tokenData->tokenName = "CHARCONST";
				break;
			case AND:
				yylval.tokenData->tokenName = "AND";
				break;
			case OR:
				yylval.tokenData->tokenName = "OR";
				break;
			case NOT:
				yylval.tokenData->tokenName = "NOT";
				break;
			case NOTEQ:
				yylval.tokenData->tokenName = "NOTEQ";
				break;
			case MULASS:
				yylval.tokenData->tokenName = "MULASS";
				break;
			case INC:
				yylval.tokenData->tokenName = "INC";
				break;
			case ADDASS:
				yylval.tokenData->tokenName = "ADDASS";
				break;
			case DEC:
				yylval.tokenData->tokenName = "DEC";
				break;
			case SUBASS:
				yylval.tokenData->tokenName = "SUBASS";
				break;
			case DIVASS:
				yylval.tokenData->tokenName = "DIVASS";
				break;
			case LESSEQ:
				yylval.tokenData->tokenName = "LESSEQ";
				break;
			case EQ:
				yylval.tokenData->tokenName = "EQ";
				break;
			case GRTEQ:
				yylval.tokenData->tokenName = "GRTEQ";
				break;
			case ID:
				yylval.tokenData->tokenName = "ID";
				break;
			case RECORD:
				yylval.tokenData->tokenName = "RECORD";
				break;
			case INT:
				yylval.tokenData->tokenName = "INT";
				break;
			case BOOL:
				yylval.tokenData->tokenName = "BOOL";
				break;
			case CHAR:
				yylval.tokenData->tokenName = "CHAR";
				break;
			case STATIC:
				yylval.tokenData->tokenName = "STATIC";
				break;
			case IF:
				yylval.tokenData->tokenName = "IF";
				break;
			case WHILE:
				yylval.tokenData->tokenName = "WHILE";
				break;
			case BREAK:
				yylval.tokenData->tokenName = "BREAK";
				break;
			case ELSE:
				yylval.tokenData->tokenName = "ELSE";
				break;
			case RETURN:
				yylval.tokenData->tokenName = "RETURN";
				break;
			case RECTYPE:
				yylval.tokenData->tokenName = "RECTYPE";
				break;
			default:
				yylval.tokenData->tokenName = strdup(yytext);
				break;
		}
		return tokenClass;
	}
	  
%}

%option noyywrap

LETTER	[a-zA-Z]
DIGIT	[0-9]
DIGLET  {DIGIT}|{LETTER}

%%

[ \t]+
"//".*
\n    				{++lineNum;}

"true"				{return setTokenData(BOOLCONST);}
"false"				{return setTokenData(BOOLCONST);}
"and"				{return setTokenData(AND);}
"or"				{return setTokenData(OR);}
"not"				{return setTokenData(NOT);}
[\+\-\*\/\?\.\%]	{return setTokenData(yytext[0]);}
[\,\;\>\<\=\:]      {return setTokenData(yytext[0]);}
[\(\)\{\}\[\]]		{return setTokenData(yytext[0]);}
"!="				{return setTokenData(NOTEQ);}
"*="				{return setTokenData(MULASS);}
"++"				{return setTokenData(INC);}
"+="				{return setTokenData(ADDASS);}
"--"				{return setTokenData(DEC);}
"-="				{return setTokenData(SUBASS);}
"/="				{return setTokenData(DIVASS);}
"<="				{return setTokenData(LESSEQ);}
"=="				{return setTokenData(EQ);}
">="				{return setTokenData(GRTEQ);}
"record"			{return setTokenData(RECORD);}
"int"				{return setTokenData(INT);}
"bool"				{return setTokenData(BOOL);}
"char"				{return setTokenData(CHAR);}
"static"			{return setTokenData(STATIC);}
"if"				{return setTokenData(IF);}
"while"				{return setTokenData(WHILE);}
"break"				{return setTokenData(BREAK);}
"else"				{return setTokenData(ELSE);}
"return"			{return setTokenData(RETURN);}
\'\\?.\'			{return setTokenData(CHARCONST);}
{DIGIT}+			{return setTokenData(NUMCONST);}
{LETTER}{DIGLET}* 	{
						if(recordSymTab.lookup(yytext) != NULL){
							return setTokenData(RECTYPE);
						}
						else
							return setTokenData(ID);
					}

<<EOF>> {return 0;}

.   {
    printf("WARNING(%d): Invalid input character: '%s'. Character ignored.\n", 
          lineNum, yytext);
		  numWarnings++;
    }


%%

