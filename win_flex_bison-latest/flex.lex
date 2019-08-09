%{
    #include "stdio.h"
    #include "stdlib.h"
	#include "Token.h"
	int yyline=1;
	#include "parser.h"
	FILE *syntactic_output = NULL, *semantic_output = NULL;
%}

PROGRAM_KEYWORD		"program"
END_KEYWORD 		"end"
VOID_KEYWORD		"void"
INT_KEYWORD			"integer"
REAL_KEYWORD		"real"
RETURN_KEYWORD		"return"
END_OF_FILE			"<<EOF>>"
DIVISION			"/"
MULTIPLICATION		"*"
ASSIGNMENT			"="
COMMA				","
SEMICOLON			";"
LPAREN				"("
RPAREN				")"
LBRACKET			"["
RBRACKET			"]"
LCURL_BRACKET		"{"
RCURL_BRACKET		"}"
ALPHANUM			[a-zA-Z0-9]
DIGIT				[0-9] 
ALPHA				[a-zA-Z]
WHITESPACE_TOKEN	[ \t]
ID					{ALPHA}("_"?{ALPHANUM})*
%%

{PROGRAM_KEYWORD}										{create_and_store_token(PROGRAM_KEYWORD,yytext,yyline);	printValid(PROGRAM_KEYWORD,yyline,yytext);		return 1;}
{END_KEYWORD}											{create_and_store_token(END_KEYWORD,yytext,yyline);		printValid(END_KEYWORD,yyline,yytext);			return 1;}
{VOID_KEYWORD}											{create_and_store_token(VOID_KEYWORD,yytext,yyline);		printValid(VOID_KEYWORD,yyline,yytext);		return 1;}
{INT_KEYWORD}											{create_and_store_token(INT_KEYWORD,yytext,yyline);		printValid(INT_KEYWORD,yyline,yytext);			return 1;}
{REAL_KEYWORD}											{create_and_store_token(REAL_KEYWORD,yytext,yyline);		printValid(REAL_KEYWORD,yyline,yytext);		return 1;}
{RETURN_KEYWORD}										{create_and_store_token(RETURN_KEYWORD,yytext,yyline);	printValid(RETURN_KEYWORD,yyline,yytext);		return 1;}
{DIVISION}												{create_and_store_token(DIVISION,yytext,yyline);			printValid(DIVISION,yyline,yytext);			return 1;}
{MULTIPLICATION}										{create_and_store_token(MULTIPLICATION,yytext,yyline);	printValid(MULTIPLICATION,yyline,yytext);		return 1;}
{ASSIGNMENT}											{create_and_store_token(ASSIGNMENT,yytext,yyline);		printValid(ASSIGNMENT,yyline,yytext);			return 1;}
{COMMA} 												{create_and_store_token(COMMA,yytext,yyline);			printValid(COMMA,yyline,yytext);				return 1;}
{SEMICOLON}												{create_and_store_token(SEMICOLON,yytext,yyline);		printValid(SEMICOLON,yyline,yytext);			return 1;}
{LPAREN}												{create_and_store_token(LPAREN,yytext,yyline);			printValid(LPAREN,yyline,yytext);				return 1;}
{RPAREN}												{create_and_store_token(RPAREN,yytext,yyline);			printValid(RPAREN,yyline,yytext);				return 1;}
{LBRACKET}												{create_and_store_token(LBRACKET,yytext,yyline);			printValid(LBRACKET,yyline,yytext);			return 1;}
{RBRACKET}												{create_and_store_token(RBRACKET,yytext,yyline);			printValid(RBRACKET,yyline,yytext);			return 1;}
{LCURL_BRACKET}											{create_and_store_token(LCURL_BRACKET,yytext,yyline);	printValid(LCURL_BRACKET,yyline,yytext);		return 1;}
{RCURL_BRACKET}											{create_and_store_token(RCURL_BRACKET,yytext,yyline);	printValid(RCURL_BRACKET,yyline,yytext);		return 1;}					
"--".*													;
0|[1-9]{DIGIT}*									   		{create_and_store_token(INTEGER,yytext,yyline);			printValid(INTEGER,yyline,yytext);				return 1;}
0"."{DIGIT}+|[1-9]{DIGIT}*"."{DIGIT}+					{create_and_store_token(REAL_NUMBER,yytext,yyline);		printValid(REAL_NUMBER,yyline,yytext);			return 1;}
{ID}													{create_and_store_token(ID,yytext,yyline);				printValid(ID,yyline,yytext);					return 1;}
"\n"													yyline++;
{WHITESPACE_TOKEN}										;
.														printError(yyline,yytext);
<<EOF>>													{create_and_store_token(END_OF_FILE,yytext,yyline);		printValid(END_OF_FILE,yyline,yytext);		yyterminate();}
%%
int main(int argc, char** argv)
{
	yyin = fopen("C:\\temp\\test1.txt", "r"); 
	yyout = fopen("c:\\temp\\test1_203974878_308470418_lex.txt", "w");  
	syntactic_output = fopen("C:\\temp\\test1_203974878_308470418_syntactic.txt", "w");
	semantic_output = fopen("C:\\temp\\test1_203974878_308470418_semantic.txt", "w");
	
	Parser();
	
	fclose(yyin);
	fclose(yyout);
	fclose(syntactic_output);
	fclose(semantic_output);
	deleteArraysAndLinkedList();
	yyline=1;

	yyin = fopen("C:\\temp\\test2.txt", "r"); 
	yyout = fopen("c:\\temp\\test2_203974878_308470418_lex.txt", "w");  
	syntactic_output = fopen("C:\\temp\\test2_203974878_308470418_syntactic.txt", "w");
	semantic_output = fopen("C:\\temp\\test2_203974878_308470418_semantic.txt", "w");
	Parser();
	
	fclose(yyin);
	fclose(yyout);
	fclose(syntactic_output);
	fclose(semantic_output);
	deleteArraysAndLinkedList();
	yyline=1;
}
int yywrap(){return 1;}