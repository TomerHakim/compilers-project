%{
#include<stdlib.h>
#include "Token.h"
#include<conio.h>
#include<stdio.h>

int line=1;

%}
%x comment1 comment2 comment3
%%
0|-?[1-9][0-9]*							{create_and_store_token(INT_NUM,yytext,line);}
-?0"."[0-9]+|-?[1-9][0-9]*"."[0-9]+				{create_and_store_token(REAL_NUM,yytext,line);}
"+"								{create_and_store_token(ADDITION,yytext,line);}
"-"								{create_and_store_token(SUBSTRACTION,yytext,line);}
"*"								{create_and_store_token(MULTIPLICATION,yytext,line);}
"/"								{create_and_store_token(DIVISION,yytext,line);}
"**"								{create_and_store_token(POWER,yytext,line);}
"++"								{create_and_store_token(UNARY,yytext,line);}
"="								{create_and_store_token(ASSIGNMENT,yytext,line);}
"!="								{create_and_store_token(NEQ,yytext,line);}
={2}								{create_and_store_token(COMPARISON,yytext,line);}
"<"								{create_and_store_token(LESS,yytext,line);}
">"								{create_and_store_token(GREATER,yytext,line);}
"<="								{create_and_store_token(LEQ,yytext,line);}
">="								{create_and_store_token(GEQ,yytext,line);}
":"								{create_and_store_token(COLON,yytext,line);}
";"								{create_and_store_token(SEMICOLON,yytext,line);}
"("								{create_and_store_token(LPAREN,yytext,line);}
")"								{create_and_store_token(RPAREN,yytext,line);}
"["								{create_and_store_token(LBRACKET,yytext,line);}
"]"								{create_and_store_token(RBRACKET,yytext,line);}
","								{create_and_store_token(COMMA,yytext,line);}
"$" 								{storeInvalidChar(INVALID,yytext,line);}
"#" 								{storeInvalidChar(INVALID,yytext,line);}
"@" 								{storeInvalidChar(INVALID,yytext,line);}
"%" 								{storeInvalidChar(INVALID,yytext,line);}
"|" 								{storeInvalidChar(INVALID,yytext,line);}
"\\" 								{storeInvalidChar(INVALID,yytext,line);}
"'" 								{storeInvalidChar(INVALID,yytext,line);}
"\"" 								{storeInvalidChar(INVALID,yytext,line);}


"^"[a-zA-Z]+[a-zA-Z0-9]*([_][a-zA-Z0-9]+)*					{create_and_store_token(POINTER,yytext,line);}
"&"[a-zA-Z]+[a-zA-Z0-9]*([_][a-zA-Z0-9]+)*						{create_and_store_token(ADDRESS,yytext,line);}
"\n"								{fprintf(yyout,"\r\n%s",yytext);line++;}
"block"								{create_and_store_token(BLOCK_TOK,yytext,line);}
"begin"								{create_and_store_token(BEGIN_TOK,yytext,line);}
"end"								{create_and_store_token(END_TOK,yytext,line);}
"type"								{create_and_store_token(TYPE_TOK,yytext,line);}
"integer"							{create_and_store_token(INTEGER_TOK,yytext,line);}
"real"								{create_and_store_token(REAL_TOK,yytext,line);}
"array"							        {create_and_store_token(ARRAY_TOK,yytext,line);}
"of"								{create_and_store_token(OF_TOK,yytext,line);}
"when"								{create_and_store_token(WHEN_TOK,yytext,line);}
"do"								{create_and_store_token(DO_TOK,yytext,line);}
"default"							{create_and_store_token(DEFAULT_TOK,yytext,line);}
"end_when"							{create_and_store_token(END_WHEN_TOK,yytext,line);}
"for"								{create_and_store_token(FOR_TOK,yytext,line);}
"end_for"							{create_and_store_token(END_FOR_TOK,yytext,line);}
"malloc"							{create_and_store_token(MALLOC_TOK,yytext,line);}
"free"								{create_and_store_token(FREE_TOK,yytext,line);}
"size_of"							{create_and_store_token(SIZE_OF_TOK,yytext,line);}
[a-zA-Z]+[a-zA-Z0-9]*([_][a-zA-Z0-9]+)*				{create_and_store_token(ID,yytext,line);}
"/*"  				BEGIN(comment1);
<comment1>[^*]*                 /*do nothing till ‘*’  */
<comment1>\n                    {line++;}/*delete new line in multi-line comment*/
<comment1>"*/"  		BEGIN(0);

"//"|"--"   				BEGIN(comment2);
<comment2>[^\n]                 /*do nothing till new line */
<comment2>\n   			{fprintf(yyout,"\r\n%s",yytext);line++;BEGIN(0);}


[ \t]


.									{BEGIN(comment3);fprintf(yyout,"\r\nThe character '%c' at line: '%d' does not begin any legal token in the language.",yytext[0],line);}
<comment3>[^" ";\t\n]					/*do nothing till new line */
<comment3>[" ";\t] 					BEGIN(0);
<comment3>[\n] 						line++;BEGIN(0);

<<EOF>>								{yyterminate();}	
%%

void main(int argc, char* argv[])
{
yyin=fopen("c:\\temp\\test1.txt","r");
yyout= fopen("c:\\temp\\test1_203502539_200876076_lex.txt", "ab+");

if (yyout) 
{
	yylex();
	print_nodes();
}

else 
{
	yyout = stdout;
	yylex();
}
line = 1;
yyrestart();
yyin = fopen("C:\\temp\\test2.txt", "r");
yyout = fopen("C:\\temp\\test2_203502539_200876076_lex.txt", "ab+");
if (yyout) 
{
	inital_all();
	BEGIN INITIAL;
	yylex();
	print_nodes();
}
else 
{
	yyout = stdout;
	BEGIN INITIAL;
	yylex();
}

}	
