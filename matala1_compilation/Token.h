#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *yyin, *yyout, *syntactic_output, *semantic_output;

typedef enum eTOKENS
{
	DIGIT,
	REAL_NUMBER,
	INTEGER,
	INT_KEYWORD,
	REAL_KEYWORD,
	PROGRAM_KEYWORD,
	END_KEYWORD,
	VOID_KEYWORD,
	RETURN_KEYWORD,
	DIVISION,
	MULTIPLICATION,
	ASSIGNMENT,
	COMMA,
	SEMICOLON,
	LPAREN,
	RPAREN,
	LBRACKET,
	RBRACKET,
	LCURL_BRACKET,
	RCURL_BRACKET,
	COMMENT,
	ID,
	END_OF_FILE
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char *lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token *tokensArray;
	struct Node *prev;
	struct Node *next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
void printError(int yyline, char* yytext);
void printValid(eTOKENS kind, int yyline, char* yytext);
const char *kindToStringConverter(eTOKENS num);
Token *next_token();
Token *back_token();
void deleteArraysAndLinkedList();

#endif