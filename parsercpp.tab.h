/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSERCPP_TAB_H_INCLUDED
# define YY_YY_PARSERCPP_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUMCONST = 258,
    CHARCONST = 259,
    BOOLCONST = 260,
    AND = 261,
    OR = 262,
    NOT = 263,
    NOTEQ = 264,
    MULASS = 265,
    INC = 266,
    ADDASS = 267,
    DEC = 268,
    SUBASS = 269,
    DIVASS = 270,
    LESSEQ = 271,
    EQ = 272,
    GRTEQ = 273,
    ID = 274,
    RECORD = 275,
    INT = 276,
    BOOL = 277,
    CHAR = 278,
    STATIC = 279,
    IF = 280,
    WHILE = 281,
    BREAK = 282,
    ELSE = 283,
    RETURN = 284,
    RECTYPE = 285
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 27 "parsercpp.y" /* yacc.c:1909  */

  TokenData* tokenData;
  TreeNode* treeNode;
  DeclKind declKind;

#line 91 "parsercpp.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSERCPP_TAB_H_INCLUDED  */
