/*
#include "Parser.h"

void Parser(){	
	Parse_PROGRAM();
	match(END_OF_FILE);
}

int match(eTOKENS t)
{
	Token *current_token;
	
	current_token=next_token();
	if(current_token->kind != t)
	{
		fprintf(syntactic_output,"Expected: token '%s' at line %d,\nActual token: '%s, lexeme'%s'.\n",kindToStringConverter(t),current_token->lineNumber,kindToStringConverter(current_token->kind),current_token->lexeme);
		return 0;
	}
	return 1;
}

void ErrorHandler(eTOKENS* follow_tokens_array, int size){
	Token *current_token=next_token(); //extra next token
	int i;

	current_token=back_token(); //fix it

	while (current_token->kind!=END_OF_FILE){ //Perform current_token=next token until you get to EOF or to one of the tokens in the follow array
		for(i=0;i<size;i++){
				if (current_token->kind==follow_tokens_array[i]){
					current_token=back_token();
					return;
			}
		}
		current_token=next_token();
	}
}

void Parse_PROGRAM(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {END_OF_FILE};

	switch (current_token->kind)
	{
	case PROGRAM_KEYWORD:
		fprintf(syntactic_output,"Rule (PROGRAM --> program VAR_DEFINITIONS; STATEMENTS end FUNC_DEFINITIONS)\n");
		Parse_VAR_DEFINITIONS();
		match(SEMICOLON);
		Parse_STATEMENTS();
		match(END_KEYWORD);
		Parse_FUNC_DEFINITIONS();
		break;
	default: 
		fprintf(syntactic_output,"Expected: one of tokens 'PROGRAM_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}


void Parse_VAR_DEFINITIONS(){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};

	switch (current_token->kind)
	{
	case REAL_KEYWORD:
	case INT_KEYWORD:
		fprintf(syntactic_output,"Rule (VAR_DEFINITIONS --> VAR_DEFINITION VAR_DEFINITIONS_A)\n");
		current_token = back_token();
		Parse_VAR_DEFINITION();
		Parse_VAR_DEFINITIONS_A();
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 2);
		break;
	}
	
}


void Parse_VAR_DEFINITIONS_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};

	switch (current_token->kind){
		case SEMICOLON:
			current_token = next_token();//peek
			if (current_token->kind == REAL_KEYWORD || current_token->kind == INT_KEYWORD) { //Non-nullable case: see if we got to firsts of "VAR_DEFINITIONS"
				fprintf(syntactic_output,"Rule (VAR_DEFINITIONS_A --> ; VAR_DEFINITIONS)\n");
				current_token = back_token();
				Parse_VAR_DEFINITIONS();
			}
			else if (current_token->kind == ID || current_token->kind == LCURL_BRACKET || current_token->kind == RETURN_KEYWORD){ // Nullable Cases: check if we got to "firsts" of STATEMENTS
				current_token = back_token();//due to peek
				current_token = back_token();//Normal nullable action
				fprintf(syntactic_output,"Rule (VAR_DEFINITIONS_A --> null)\n");
			}
			else {
					fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD', 'INT_KEYWORD', 'ID', 'LCURL_BRACKET', 'RETURN_KEYWORD' at line: %d \n",current_token->lineNumber);
					fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
					ErrorHandler(follow_array, 2);
			}
			break;
		case RPAREN:
			fprintf(syntactic_output,"Rule (VAR_DEFINITIONS_A --> null)\n");
			current_token = back_token();//Normal nullable action
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'SEMICOLON', 'RPAREN' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}
}


void Parse_VAR_DEFINITION(){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};

	switch (current_token->kind) {
		case REAL_KEYWORD:
		case INT_KEYWORD: 
			fprintf(syntactic_output,"Rule (VAR_DEFINITION --> TYPE VARIABLES_LIST)\n");
			current_token = back_token();
			Parse_TYPE();
			Parse_VARIABLES_LIST();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}	
}


void Parse_TYPE(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {ID};

	switch (current_token->kind)
	{
		case REAL_KEYWORD:
			fprintf(syntactic_output,"Rule (TYPE --> real)\n");
				break;
		case INT_KEYWORD:
			fprintf(syntactic_output,"Rule (TYPE --> integer)\n");
				break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}
}


void Parse_VARIABLES_LIST(){
	Token *current_token=next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};

	switch (current_token->kind){
		case ID:		
			fprintf(syntactic_output,"Rule (VARIABLES_LIST --> VARIABLE VARIABLES_LIST_A)\n");
			current_token=back_token();
			Parse_VARIABLE();
			Parse_VARIABLES_LIST_A();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'ID', at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}
}

*/
/*
void Parse_VARIABLES_LIST_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};

	switch (current_token->kind){
		case COMMA:
			fprintf(syntactic_output,"Rule (VARIABLES_LIST_A --> , VARIABLE VARIABLES_LIST_A)\n");
			Parse_VARIABLE();
			Parse_VARIABLES_LIST_A();
			break;
		//Nullable Cases:
		case SEMICOLON:
		case RPAREN:
			fprintf(syntactic_output,"Rule (VARIABLES_LIST_A --> null)\n");
			current_token = back_token();
			break;

		default:
			fprintf(syntactic_output,"Expected: one of tokens 'COMMA', 'SEMICOLON', 'RPAREN'  at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}
}


void Parse_VARIABLE(){
	Token *current_token = next_token();
	eTOKENS follow_array[4] = {ASSIGNMENT,COMMA,SEMICOLON,RPAREN};

	switch (current_token->kind){
	case ID:
		fprintf(syntactic_output,"Rule (VARIABLE --> id  VARIABLE_A)\n");
		Parse_VARIABLE_A();
		break;	
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 4);
		break;
	}
}


void Parse_VARIABLE_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[4] = {ASSIGNMENT,COMMA,SEMICOLON,RPAREN};

	switch (current_token->kind){
	case LBRACKET:
		fprintf(syntactic_output,"Rule (VARIABLE_A -->  [int_number])\n");
		match(INTEGER);
		match(RBRACKET);
		break;
	// Nullable Cases: 
	case ASSIGNMENT:
	case COMMA:
	case SEMICOLON:
	case RPAREN:
		fprintf(syntactic_output,"Rule (VARIABLE_A --> null)\n");
		current_token = back_token();
		break;

	default:
		fprintf(syntactic_output,"Expected: one of tokens 'LBRACKET', 'ASSIGNMENT','COMMA', 'SEMICOLON', 'RPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 4);
		break;
	}
}


void Parse_FUNC_DEFINITIONS(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {END_OF_FILE};

	switch (current_token->kind){
	case VOID_KEYWORD:
	case INT_KEYWORD:
	case REAL_KEYWORD:
		fprintf(syntactic_output,"Rule (FUNC_DEFINITIONS --> FUNC_DEFINITION FUNC_DEFINITIONS_A)\n");
		current_token=back_token();
		Parse_FUNC_DEFINITION();
		Parse_FUNC_DEFINITIONS_A();
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'VOID_KEYWORD', 'INT_KEYWORD', 'REAL_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"\r\nActual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}


void Parse_FUNC_DEFINITIONS_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {END_OF_FILE};
	
	switch (current_token->kind){
		case VOID_KEYWORD:
		case INT_KEYWORD:
		case REAL_KEYWORD:
			fprintf(syntactic_output,"Rule (FUNC_DEFINITIONS_A --> FUNC_DEFINITION FUNC_DEFINITIONS_A)\n");
			current_token=back_token();
			Parse_FUNC_DEFINITION();
			Parse_FUNC_DEFINITIONS_A();
			break;
		case END_OF_FILE:
			fprintf(syntactic_output,"Rule (FUNC_DEFINITIONS_A --> null)\n");
			current_token=back_token();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'VOID_KEYWORD', 'INT_KEYWORD', 'REAL_KEYWORD', 'END_OF_FILE' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}
}


void Parse_FUNC_DEFINITION(){
	Token *current_token = next_token();
	eTOKENS follow_array[3] = {VOID_KEYWORD,INT_KEYWORD,REAL_KEYWORD};

	switch (current_token->kind){
		case VOID_KEYWORD:
		case INT_KEYWORD:
		case REAL_KEYWORD:
			fprintf(syntactic_output,"Rule (FUNC_DEFINITION --> RETURNED_TYPE  id  (PARAM_DEFINITIONS)  BLOCK)\n");
			current_token = back_token();
			Parse_RETURNED_TYPE();
			match(ID);
			match(LPAREN);
			Parse_PARAM_DEFINITIONS();
			match(RPAREN);
			Parse_BLOCK();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'VOID_KEYWORD', 'INT_KEYWORD', 'REAL_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token '%s',lexeme:'%s \n'.",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 3);
			break;
	}
}


void Parse_RETURNED_TYPE(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {ID};

	switch (current_token->kind)
	{
		case VOID_KEYWORD:
			fprintf(syntactic_output,"Rule (RETURNED_TYPE --> void)\n");
			break;
		case REAL_KEYWORD:
		case INT_KEYWORD:
			fprintf(syntactic_output,"Rule (RETURNED_TYPE --> TYPE)\n");
			current_token = back_token();
			Parse_TYPE();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'VOID_KEYWORD', 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}
}


void Parse_PARAM_DEFINITIONS(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {RPAREN};
	
	switch(current_token->kind){
	case REAL_KEYWORD:
	case INT_KEYWORD:
		fprintf(syntactic_output,"Rule (PARAM_DEFINITIONS --> VAR_DEFINITIONS)\n");
		current_token = back_token();
		Parse_VAR_DEFINITIONS();
		break;
	case RPAREN:
		fprintf(syntactic_output,"Rule (PARAM_DEFINITIONS --> null)\n");
		current_token=back_token();
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD', 'RPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}


void Parse_STATEMENTS(){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {END_OF_FILE,RCURL_BRACKET};

	switch (current_token->kind){
	case ID:
	case LCURL_BRACKET:
	case RETURN_KEYWORD:
		fprintf(syntactic_output,"Rule (STATEMENTS --> STATEMENT; STATEMENTS_A)\n");
		current_token=back_token();
		Parse_STATEMENT();
		match(SEMICOLON);
		Parse_STATEMENTS_A();
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID','LCURL_BRACKET', 'RETURN_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 2);
		break;
	}	
}


void Parse_STATEMENTS_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {END_OF_FILE,RCURL_BRACKET};

	switch(current_token->kind){
		case ID:
		case LCURL_BRACKET:
		case RETURN_KEYWORD:
			fprintf(syntactic_output,"Rule (STATEMENTS_A --> STATEMENTS)\n");
			current_token=back_token();
			Parse_STATEMENTS();
			break;
		case END_KEYWORD:
		case RCURL_BRACKET:
			fprintf(syntactic_output,"Rule (STATEMENTS_A --> null)\n");
			current_token=back_token();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'ID','LCURL_BRACKET', 'RETURN_KEYWORD', 'END_KEYWORD', 'RCURL_BRACKET' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}
}


void Parse_STATEMENT(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	
	switch (current_token->kind){
		case LCURL_BRACKET:
			fprintf(syntactic_output,"Rule (STATEMENT --> BLOCK)\n");
			current_token = back_token();
			Parse_BLOCK();
			break;
		case RETURN_KEYWORD:
			fprintf(syntactic_output,"Rule (STATEMENT --> return STATEMENT_A)\n");
			Parse_STATEMENT_A();
			break;
		case ID:
			fprintf(syntactic_output,"Rule (STATEMENT --> id STATEMENT_B)\n");
			Parse_STATEMENT_B();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'LCURL_BRACKET', 'RETURN_KEYWORD', 'ID' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}
}


void Parse_STATEMENT_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	
	switch (current_token->kind){
	case INTEGER:
	case REAL_NUMBER:
	case ID:
		fprintf(syntactic_output,"Rule (STATEMENT_A --> EXPRESSION)\n");
		current_token = back_token();
		Parse_EXPRESSION();
		break;
	case SEMICOLON:
		// Nullable Case:
		fprintf(syntactic_output,"Rule (STATEMENT_A --> null)\n");
		current_token = back_token();	
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'INTEGER', 'REAL_NUMBER', 'ID', 'SEMICOLON' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}


void Parse_STATEMENT_B(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	
	switch (current_token->kind){
	case ASSIGNMENT:
	case LBRACKET:
		fprintf(syntactic_output,"Rule (STATEMENT_B --> VARIABLE_A = EXPRESSION)\n");
		current_token=back_token();
		Parse_VARIABLE_A();
		match(ASSIGNMENT);
		Parse_EXPRESSION();
		break;
	case LPAREN:
		fprintf(syntactic_output,"Rule (STATEMENT_B --> (PARAMETERS_LIST) )\n");
		Parse_PARAMETERS_LIST();
		match(RPAREN);
		break;
	default:
		ErrorHandler(follow_array, 1);
		fprintf(syntactic_output,"Expected: one of tokens 'ASSIGNMENT', 'LBRACKET', 'LPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		break;
	}
}


void Parse_BLOCK(){
	Token *current_token = next_token();
	eTOKENS follow_array[5] = {END_OF_FILE, SEMICOLON, VOID_KEYWORD, REAL_KEYWORD,INT_KEYWORD};

	switch (current_token->kind){
	case LCURL_BRACKET:
		fprintf(syntactic_output,"Rule (BLOCK --> { VAR_DEFINITIONS; STATEMENTS } )\n");
		Parse_VAR_DEFINITIONS();
		match(SEMICOLON);
		Parse_STATEMENTS();
		match(RCURL_BRACKET);
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'LCURL_BRACKET' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"\r\nActual token '%s',lexeme:'%s'.",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 5);
		break;
	}		
}


void Parse_FUNCTION_CALL(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};

	switch (current_token->kind){
	case ID:
		match(LPAREN);
		Parse_PARAMETERS_LIST();
		match(RPAREN);
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"\r\nActual token '%s',lexeme:'%s'.",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}


void Parse_PARAMETERS_LIST(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {RPAREN};
	
	switch(current_token->kind){
	case ID:
		fprintf(syntactic_output,"Rule (PARAMETERS_LIST --> VARIABLES_LIST)\n");
		current_token=back_token();
		Parse_VARIABLES_LIST();
		break;
	case RPAREN:
		fprintf(syntactic_output,"Rule (PARAMETERS_LIST --> null)\n");
		current_token=back_token();
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID', 'RPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}


void Parse_EXPRESSION(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};

	switch (current_token->kind){
		case ID:
			fprintf(syntactic_output,"Rule (EXPRESSION --> id EXPRESSION_A)\n");
			Parse_EXPRESSION_A();
			break;
		case INTEGER:
			fprintf(syntactic_output,"Rule (EXPRESSION --> int_number)\n");
			break;
		case REAL_NUMBER:
			fprintf(syntactic_output,"Rule (EXPRESSION --> real_number)\n");
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'ID', 'INTEGER', 'REAL_NUMBER' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}
}

void Parse_EXPRESSION_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	
	switch (current_token->kind){
		case LBRACKET:
			fprintf(syntactic_output,"Rule (EXPRESSION_A --> VARIABLE_A)\n");
			current_token = back_token();
			Parse_VARIABLE_A();
			break;
		case DIVISION:
		case MULTIPLICATION:
			fprintf(syntactic_output,"Rule (EXPRESSION_A --> ar_op EXPRESSION)\n");
			Parse_EXPRESSION();
			break;
		case SEMICOLON:
			fprintf(syntactic_output,"Rule (EXPRESSION_A --> VARIABLE_A)\n");
			current_token = back_token();
			Parse_VARIABLE_A();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'LBRACKET', 'ar_op', 'SEMICOLON' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"\r\nActual token '%s',lexeme:'%s'.",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}
}
*/