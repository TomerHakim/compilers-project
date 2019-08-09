#include "Token.h"
#include "Parser.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/

int currentIndex = 0, backStepsTaken = 0;
Node* currentNode = NULL;
#define TOKEN_ARRAY_SIZE 100

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{
	int length = strlen(lexeme) + 1;

	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray = (Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exsits in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = (Token*)calloc(sizeof(Token), TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}

	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;

	currentNode->tokensArray[currentIndex].lexeme = (char*)calloc(sizeof(char),length);
#ifdef _WIN32
	strcpy_s(currentNode->tokensArray[currentIndex].lexeme, length, lexeme);
#else
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
#endif		
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exsits).
*/
Token *back_token(){ 
	currentIndex--;
	backStepsTaken++;

	if (currentIndex < 0) { //If we were in the first token of the array and went one more step backwards 
		if (currentNode->prev == NULL) { //If the current node was also the first node (meaning, no nodes before it) --> return null
			currentIndex = 0;
			return NULL;
		}
		else { //If the current node wasn't the first node --> go one node bacwards and to the last token of it's array0
		currentIndex = TOKEN_ARRAY_SIZE-1;
		currentNode = currentNode->prev;
		}
	}
	return &(currentNode->tokensArray[currentIndex]);
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token): 
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/


Token *next_token(){ 
	if (backStepsTaken == 0) { //If the current token is the last token in the current array --> Use yylex() to fetch and store new token
		yylex();
	}
	else { //If the current token is not the last token in the current tokens array --> go one step further in the array withou using yylex(), so we won't override an existing token
		currentIndex++;
		if (currentIndex > TOKEN_ARRAY_SIZE-1) { //If we crossed the last token in the tokens array --> go to the first token in the next node. 
			currentNode = currentNode->next;
			currentIndex = 0;
		}
		backStepsTaken--; //Since we moved one step further in the array, reduce the number of back steps taken.
	}
	return &(currentNode->tokensArray[currentIndex]);
}

void deleteArraysAndLinkedList(){
	int i;

	if (currentNode==NULL){
		return;
	}
	
	while (currentNode->next){ //Go to the end of the list
		currentNode=currentNode->next;
	}

	while (currentNode->prev){ //Delete every array and node from the end and up to the first node
		/*for (i=0; i<TOKEN_ARRAY_SIZE;i++){
				free(currentNode->tokensArray[i].lexeme); //Delete each lexemme
		}
		*/
		for (currentIndex; currentIndex>-1;currentIndex--){
			free(currentNode->tokensArray[currentIndex].lexeme); //Delete each lexemme
		}
		currentIndex=TOKEN_ARRAY_SIZE-1;
		free(currentNode->tokensArray); //Delete the array
		currentNode=currentNode->prev;
		free(currentNode->next);
	}

		free(currentNode->tokensArray); //Delete the first node and it's array -> see if it can be added to the while above
		free(currentNode);
	
		currentNode=NULL;
		currentIndex=0;
		backStepsTaken=0;
		return;
}

void printError(int yyline, char* yytext) {
	fprintf(yyout, "The character '{%s}' at line: {%d} does not begin any legal token in the language.\n", yytext, yyline);
}

const char *kindToStringConverter(eTOKENS num){
	char* valid_tokens[] = { "DIGIT","REAL_NUMBER","INTEGER","INT_KEYWORD","REAL_KEYWORD","PROGRAM_KEYWORD","END_KEYWORD","VOID_KEYWORD","RETURN_KEYWORD",
		"DIVISION","MULTIPLICATION","ASSIGNMENT","COMMA","SEMICOLON","LPAREN","RPAREN","LBRACKET","RBRACKET","LCURL_BRACKET","RCURL_BRACKET",
		"COMMENT","ID","END_OF_FILE" };

	return valid_tokens[num];
}

void printValid(eTOKENS kind, int yyline, char* yytext) {
	char* valid_tokens[] = { "DIGIT","REAL_NUMBER","INTEGER","INT_KEYWORD","REAL_KEYWORD","PROGRAM_KEYWORD","END_KEYWORD","VOID_KEYWORD","RETURN_KEYWORD",
		"DIVISION","MULTIPLICATION","ASSIGNMENT","COMMA","SEMICOLON","LPAREN","RPAREN","LBRACKET","RBRACKET","LCURL_BRACKET","RCURL_BRACKET",
		"COMMENT","ID","END_OF_FILE" };

	char* cur_token = valid_tokens[kind];

	fprintf(yyout, "Token of kind '{%s}' was found at line : {%d}, lexeme : '{%s}'.\n", cur_token, yyline, yytext );
}