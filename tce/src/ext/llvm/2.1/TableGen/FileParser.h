/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     BIT = 259,
     STRING = 260,
     BITS = 261,
     LIST = 262,
     CODE = 263,
     DAG = 264,
     CLASS = 265,
     DEF = 266,
     MULTICLASS = 267,
     DEFM = 268,
     FIELD = 269,
     LET = 270,
     IN = 271,
     CONCATTOK = 272,
     SHLTOK = 273,
     SRATOK = 274,
     SRLTOK = 275,
     STRCONCATTOK = 276,
     INTVAL = 277,
     ID = 278,
     VARNAME = 279,
     STRVAL = 280,
     CODEFRAGMENT = 281
   };
#endif
/* Tokens.  */
#define INT 258
#define BIT 259
#define STRING 260
#define BITS 261
#define LIST 262
#define CODE 263
#define DAG 264
#define CLASS 265
#define DEF 266
#define MULTICLASS 267
#define DEFM 268
#define FIELD 269
#define LET 270
#define IN 271
#define CONCATTOK 272
#define SHLTOK 273
#define SRATOK 274
#define SRLTOK 275
#define STRCONCATTOK 276
#define INTVAL 277
#define ID 278
#define VARNAME 279
#define STRVAL 280
#define CODEFRAGMENT 281




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 210 "/home/elhigu/projects/llvm-trunk/merge-temp/llvm-frontend/llvm/utils/TableGen/FileParser.y"
{
  std::string*                StrVal;
  int                         IntVal;
  llvm::RecTy*                Ty;
  llvm::Init*                 Initializer;
  std::vector<llvm::Init*>*   FieldList;
  std::vector<unsigned>*      BitList;
  llvm::Record*               Rec;
  std::vector<llvm::Record*>* RecList;
  SubClassRefTy*              SubClassRef;
  std::vector<SubClassRefTy>* SubClassList;
  std::vector<std::pair<llvm::Init*, std::string> >* DagValueList;
}
/* Line 1489 of yacc.c.  */
#line 115 "FileParser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE Filelval;

