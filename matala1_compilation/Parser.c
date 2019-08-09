#include "Parser.h"

table_ptr currentTable = NULL;

//1
void Parser(){
	currentTable = make_table(currentTable);
	Parse_PROGRAM();
	match(END_OF_FILE);
	check_if_all_variables_are_used(currentTable);
	currentTable = pop_table(currentTable);
}

//2
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

//3
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

//4
void Parse_PROGRAM(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {END_OF_FILE};
	int is_parameter = 0;
	elm_type func_type = Void;
	
	switch (current_token->kind)
	{
	case PROGRAM_KEYWORD:
		fprintf(syntactic_output,"Rule (PROGRAM --> program VAR_DEFINITIONS; STATEMENTS end FUNC_DEFINITIONS)\n");
		currentTable = make_table(currentTable);
		Parse_VAR_DEFINITIONS(is_parameter);
		match(SEMICOLON);
		Parse_STATEMENTS(func_type);
		match(END_KEYWORD);
		check_if_all_variables_are_used(currentTable);
		currentTable = pop_table(currentTable);
		Parse_FUNC_DEFINITIONS();
		break;
	default: 
		fprintf(syntactic_output,"Expected: one of tokens 'PROGRAM_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}

//5
int Parse_VAR_DEFINITIONS(int is_parameter){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};
	int count=0;

	switch (current_token->kind)
	{
	case REAL_KEYWORD:
	case INT_KEYWORD:
		fprintf(syntactic_output,"Rule (VAR_DEFINITIONS --> VAR_DEFINITION VAR_DEFINITIONS_A)\n");
		current_token = back_token();
		count = (
		Parse_VAR_DEFINITION(is_parameter) +
		Parse_VAR_DEFINITIONS_A(is_parameter)
		);
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 2);
		break;
	}

	return count;
	
}

//6
int Parse_VAR_DEFINITIONS_A(int is_parameter){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};
	int count=0;


	switch (current_token->kind){
		case SEMICOLON:
			current_token = next_token();//peek
			if (current_token->kind == REAL_KEYWORD || current_token->kind == INT_KEYWORD) { //Non-nullable case: see if we got to firsts of "VAR_DEFINITIONS"
				fprintf(syntactic_output,"Rule (VAR_DEFINITIONS_A --> ; VAR_DEFINITIONS)\n");
				current_token = back_token();
				count = Parse_VAR_DEFINITIONS(is_parameter);
			}
			else if (current_token->kind == ID || current_token->kind == LCURL_BRACKET || current_token->kind == RETURN_KEYWORD){ // Nullable Cases: check if we got to "firsts" of STATEMENTS
				current_token = back_token();//due to peek
				current_token = back_token();//Normal nullable action
				fprintf(syntactic_output,"Rule (VAR_DEFINITIONS_A --> null)\n");
				count = 0;
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
	return count;
}

//7
int Parse_VAR_DEFINITION(int is_parameter){
	elm_type current_type;
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};
	int count=0, is_used=0;

	switch (current_token->kind) {
		case REAL_KEYWORD:
		case INT_KEYWORD: 
			fprintf(syntactic_output,"Rule (VAR_DEFINITION --> TYPE VARIABLES_LIST)\n");
			current_token = back_token();
			current_type = Parse_TYPE();
			count = Parse_VARIABLES_LIST(current_type,is_parameter,is_used);
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}
	return count;
}

//8
//Semantic approved
elm_type Parse_TYPE(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {ID};

	switch (current_token->kind)
	{
		case REAL_KEYWORD:
			fprintf(syntactic_output,"Rule (TYPE --> real)\n");
			return real; 
			break;
		case INT_KEYWORD:
			fprintf(syntactic_output,"Rule (TYPE --> integer)\n");
			return integer;
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			return error_type;
			break;
	}
}

//9
int Parse_VARIABLES_LIST(elm_type var_type, int is_parameter, int is_used){
	Token *current_token=next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};
	int count=0;

	switch (current_token->kind){
		case ID:		
			fprintf(syntactic_output,"Rule (VARIABLES_LIST --> VARIABLE VARIABLES_LIST_A)\n");
			current_token=back_token();
			Parse_VARIABLE(var_type,is_parameter, is_used);
			count = (Parse_VARIABLES_LIST_A(var_type,is_parameter, is_used) + 1); //need to check if it's correct assingment
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'ID', at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}

	return count;
}

//10
int Parse_VARIABLES_LIST_A(elm_type var_type, int is_parameter, int is_used){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {SEMICOLON,RPAREN};
	int count=0;

	switch (current_token->kind){
		case COMMA:
			fprintf(syntactic_output,"Rule (VARIABLES_LIST_A --> , VARIABLE VARIABLES_LIST_A)\n");
			Parse_VARIABLE(var_type,is_parameter, is_used);
			count = (Parse_VARIABLES_LIST_A(var_type,is_parameter, is_used) + 1);//check if assin=gnment is correct
			break;
		/* Nullable Cases: */
		case SEMICOLON:
		case RPAREN:
			fprintf(syntactic_output,"Rule (VARIABLES_LIST_A --> null)\n");
			current_token = back_token();
			count =0;
			break;

		default:
			fprintf(syntactic_output,"Expected: one of tokens 'COMMA', 'SEMICOLON', 'RPAREN'  at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 2);
			break;
	}
	return count;
}

//11
void Parse_VARIABLE(elm_type var_type, int is_parameter, int is_used){
	Token *temp_token=NULL;
	Token *current_token = next_token();
	table_entry temp_entry=NULL;
	eTOKENS follow_array[4] = {ASSIGNMENT,COMMA,SEMICOLON,RPAREN};
	int size;

	switch (current_token->kind){
	case ID:
		fprintf(syntactic_output,"Rule (VARIABLE --> id  VARIABLE_A)\n");
		temp_token = back_token();
		temp_token = next_token();
		temp_entry = insert(currentTable,temp_token->lexeme);		
		size = Parse_VARIABLE_A();
		if (temp_entry){
				temp_entry->name = current_token->lexeme;
				temp_entry->size = size;
				temp_entry->var_type = var_type;
				temp_entry->is_used = is_used;
				temp_entry->is_function=0;
				temp_entry->line_number=current_token->lineNumber;
				temp_entry->is_parameter=is_parameter;
			}
		else {
				if (is_used==0) {
				fprintf(semantic_output,"ERROR: Duplicated definition of variable '%s' in line number %d.\n", current_token->lexeme,current_token->lineNumber);
				}
				else {
					temp_entry = find(currentTable,temp_token->lexeme);
					if (temp_entry) {
						temp_entry->is_used=is_used;
					}
				}
			}
		//print_entries();
		break;	
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 4);
		break;
	}
}

//12
//Semantic approved
int Parse_VARIABLE_A(){
	Token *temp_token=NULL;
	Token *current_token = next_token();
	eTOKENS follow_array[4] = {ASSIGNMENT,COMMA,SEMICOLON,RPAREN};
	int size = 0; // if size is 0 - this is a single variable, if not this is an array

	switch (current_token->kind){
	case LBRACKET:
		fprintf(syntactic_output,"Rule (VARIABLE_A -->  [int_number])\n");
		match(INTEGER);
		//This is an addition, consider removing it and make 'current_token' a global pararmeter
		temp_token = back_token();
		temp_token = next_token();
		size = atoi(temp_token->lexeme);
		//end
		match(RBRACKET);
		break;
	/* Nullable Cases: */
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

	return size;
}

//13
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

//14
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

//15
void Parse_FUNC_DEFINITION(){
	Token *temp_token=NULL;
	Token *current_token = next_token();
	eTOKENS follow_array[3] = {VOID_KEYWORD,INT_KEYWORD,REAL_KEYWORD};
	elm_type func_type;
	table_entry temp_entry=NULL, temp_entry_copy=NULL;

	switch (current_token->kind){
		case VOID_KEYWORD:
		case INT_KEYWORD:
		case REAL_KEYWORD:
			fprintf(syntactic_output,"Rule (FUNC_DEFINITION --> RETURNED_TYPE  id  (PARAM_DEFINITIONS)  BLOCK)\n");
			current_token = back_token();
			func_type=Parse_RETURNED_TYPE();
			match(ID);
			//This is an addition, consider removing it and make 'current_token' a global pararmeter
			temp_token = back_token();
			temp_token = next_token();
			temp_entry = find(currentTable,temp_token->lexeme);
			if (temp_entry) {
				fprintf(semantic_output,"ERROR: Duplicated definition of function '%s' in line number: %d.\n",temp_entry->name,temp_token->lineNumber);
			}
			else {
				temp_entry = insert(currentTable,temp_token->lexeme);
				if (temp_entry){
					temp_entry->is_function=1;
					temp_entry->is_used=0;
					temp_entry->name=temp_token->lexeme;
					temp_entry->line_number=temp_token->lineNumber;
					temp_entry->var_type = func_type; 
					temp_entry->return_appeared = 0;
				}
			}
			
			match(LPAREN);
			temp_entry->num_of_params=Parse_PARAM_DEFINITIONS(); //otherwise parameters will be looked as functions
			match(RPAREN);
	
			Parse_BLOCK(func_type);
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'VOID_KEYWORD', 'INT_KEYWORD', 'REAL_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token '%s',lexeme:'%s \n'.",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 3);
			break;
	}
}

//16
elm_type Parse_RETURNED_TYPE(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {ID};
	elm_type returned_type;

	switch (current_token->kind)
	{
		case VOID_KEYWORD:
			fprintf(syntactic_output,"Rule (RETURNED_TYPE --> void)\n");
			returned_type = Void;
			break;
		case REAL_KEYWORD:
		case INT_KEYWORD:
			fprintf(syntactic_output,"Rule (RETURNED_TYPE --> TYPE)\n");
			current_token = back_token();
			returned_type = Parse_TYPE();
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'VOID_KEYWORD', 'REAL_KEYWORD','INT_KEYWORD' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}

	return returned_type;
}

//17
int Parse_PARAM_DEFINITIONS(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {RPAREN};
	int count=0;
	int is_parameter=1;
	
	switch(current_token->kind){
	case REAL_KEYWORD:
	case INT_KEYWORD:
		fprintf(syntactic_output,"Rule (PARAM_DEFINITIONS --> VAR_DEFINITIONS)\n");
		current_token = back_token();
		count = Parse_VAR_DEFINITIONS(is_parameter);
		break;
	case RPAREN:
		fprintf(syntactic_output,"Rule (PARAM_DEFINITIONS --> null)\n");
		current_token=back_token();
		count = 0;
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'REAL_KEYWORD','INT_KEYWORD', 'RPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}

	return count;
}

//18
void Parse_STATEMENTS(elm_type func_type){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {END_OF_FILE,RCURL_BRACKET};

	switch (current_token->kind){
	case ID:
	case LCURL_BRACKET:
	case RETURN_KEYWORD:
		fprintf(syntactic_output,"Rule (STATEMENTS --> STATEMENT; STATEMENTS_A)\n");
		current_token=back_token();
		Parse_STATEMENT(func_type);
		match(SEMICOLON);
		Parse_STATEMENTS_A(func_type);
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID','LCURL_BRACKET', 'RETURN_KEYWORD' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 2);
		break;
	}	
}

//19
void Parse_STATEMENTS_A(elm_type func_type){
	Token *current_token = next_token();
	eTOKENS follow_array[2] = {END_OF_FILE,RCURL_BRACKET};

	switch(current_token->kind){
		case ID:
		case LCURL_BRACKET:
		case RETURN_KEYWORD:
			fprintf(syntactic_output,"Rule (STATEMENTS_A --> STATEMENTS)\n");
			current_token=back_token();
			Parse_STATEMENTS(func_type);
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

//20
void Parse_STATEMENT(elm_type func_type){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	elm_type returned_type;
	table_entry temp_entry=NULL, original_entry=NULL;
	int is_function=0;

	switch (current_token->kind){
		case LCURL_BRACKET:
			fprintf(syntactic_output,"Rule (STATEMENT --> BLOCK)\n");
			current_token = back_token();
			Parse_BLOCK(func_type);
			break;
		case RETURN_KEYWORD:
			fprintf(syntactic_output,"Rule (STATEMENT --> return STATEMENT_A)\n");
			returned_type = Parse_STATEMENT_A();
			temp_entry=find_function_entry(currentTable);
			if (temp_entry) {
				temp_entry->return_appeared=1;
				original_entry = find(currentTable->next_table, temp_entry->name);
				if (original_entry) {
					original_entry->return_appeared=1;
				}
			}
			if (returned_type!=func_type) {
				fprintf(semantic_output,"ERROR: Returned type: %s differs from function type: %s. Line number: %d \n", typeToStringConverter(returned_type), typeToStringConverter(func_type),current_token->lineNumber);
			}
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

//21
elm_type Parse_STATEMENT_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	elm_type returned_type;
	
	switch (current_token->kind){
	case INTEGER:
	case REAL_NUMBER:
	case ID:
		fprintf(syntactic_output,"Rule (STATEMENT_A --> EXPRESSION)\n");
		current_token = back_token();
		returned_type = Parse_EXPRESSION();
		break;
	case SEMICOLON:
		/* Nullable Case: */
		fprintf(syntactic_output,"Rule (STATEMENT_A --> null)\n");
		current_token = back_token();
		returned_type = Void;
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'INTEGER', 'REAL_NUMBER', 'ID', 'SEMICOLON' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		returned_type = Void;
		break;		
	}
	return returned_type;
}

//22
void Parse_STATEMENT_B(){
	Token *current_token = next_token();
	Token* temp_token=NULL;
	eTOKENS follow_array[1] = {SEMICOLON};
	table_entry current_entry = NULL;
	int temp_size, current_array_index, num_of_called_parameters;
	elm_type current_expression_type;
	
	switch (current_token->kind){
	case ASSIGNMENT:
	case LBRACKET:
		fprintf(syntactic_output,"Rule (STATEMENT_B --> VARIABLE_A = EXPRESSION)\n");
		current_token=back_token();
		temp_size = Parse_VARIABLE_A();
		current_entry=find(currentTable,current_token->lexeme);
			if(!find(currentTable,current_token->lexeme)) {
				fprintf(semantic_output, "ERROR: The variable '%s' is in use, but was not declared. Line number: %d\n",current_token->lexeme,current_token->lineNumber);
			}
			else /*if (current_entry)*/ {
				current_entry->is_used=1;
				if (current_entry->size>0){
					temp_token = next_token();
					temp_token = back_token();
					if (temp_token->kind == ID) {
						fprintf(semantic_output,"ERROR: Direct assignment to an array id is forbidden. Id: %s, line number: %d. \n",current_entry->name, current_token->lineNumber);
					}

					temp_token = back_token();
					current_array_index = atoi(temp_token->lexeme);
					if (current_array_index>current_entry->size) {
						fprintf(semantic_output,"ERROR: Array index out of bound. Id: %s, array size: %d, but required index: %d line number: %d. \n",current_entry->name, current_entry->size, current_array_index, current_token->lineNumber);
					}
					temp_token = next_token();
				}
			
			if (current_entry->is_function==1) {
				fprintf(semantic_output,"ERROR: Direct assignment to a function id is forbidden. Id: %s, line number: %d. \n",current_entry->name, current_token->lineNumber);
			}
		}
		match(ASSIGNMENT);
		current_expression_type = Parse_EXPRESSION();
		if (current_entry) {
			if (current_entry->var_type!= current_expression_type) { //check if the id type is different than the expression type (id = expression)
				if (!(current_entry->var_type==real && current_expression_type==integer)){ // one exception is allowed --> when id type = real and expression type = integer
					fprintf(semantic_output,"ERROR: Type mismatch. Variable: %s type is %s which is different than the expression type: %s. Line number: %d. \n",current_entry->name, typeToStringConverter(current_entry->var_type), typeToStringConverter(current_expression_type), current_token->lineNumber);
					}
				}
		}
		break;
	case LPAREN: //function call
		fprintf(syntactic_output,"Rule (STATEMENT_B --> (PARAMETERS_LIST) )\n");
		temp_token = back_token();
		current_entry=find(currentTable,temp_token->lexeme);
		if (!current_entry) {
			fprintf(semantic_output, "ERROR: The function '%s' is in use, but was not declared. Line number: %d \n",temp_token->lexeme,temp_token->lineNumber);
		}
		temp_token = next_token();
		num_of_called_parameters = Parse_PARAMETERS_LIST();
		if (current_entry) {
				current_entry->is_used=1;
				if (current_entry->num_of_params!=num_of_called_parameters) {
				fprintf(semantic_output, "ERROR: The number of parameters used in function call '%s' does not match to the function's definition. Required number of parameters: %d, given number of parameters %d. Line number: %d. \n",current_entry->name,current_entry->num_of_params,num_of_called_parameters,current_token->lineNumber);
				}
		}		
		match(RPAREN);
		break;
	default:
		ErrorHandler(follow_array, 1);
		fprintf(syntactic_output,"Expected: one of tokens 'ASSIGNMENT', 'LBRACKET', 'LPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token: '%s',lexeme:'%s'.\n",kindToStringConverter(current_token->kind),current_token->lexeme);
		break;
	}
}

//23
void Parse_BLOCK(elm_type func_type){
	Token *current_token = next_token();
	eTOKENS follow_array[5] = {END_OF_FILE, SEMICOLON, VOID_KEYWORD, REAL_KEYWORD,INT_KEYWORD};
	int is_function=0, is_parameter =0;

	switch (current_token->kind){
	case LCURL_BRACKET:
		fprintf(syntactic_output,"Rule (BLOCK --> { VAR_DEFINITIONS; STATEMENTS } )\n");
			currentTable=make_table(currentTable);
			add_previous_functions_entries_to_sun_table(currentTable);
			add_parameters_to_function(currentTable);
		Parse_VAR_DEFINITIONS(is_parameter);
		match(SEMICOLON);
		Parse_STATEMENTS(func_type);
		match(RCURL_BRACKET);
			check_if_all_variables_are_used(currentTable);
			check_if_return_appeared_in_block(currentTable);
			currentTable=pop_table(currentTable);
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'LCURL_BRACKET' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"\r\nActual token '%s',lexeme:'%s'.",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 5);
		//currentTable=pop_table(currentTable);
		break;
	}		
}

//24
void Parse_FUNCTION_CALL(){
	Token *temp_token = NULL;
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	table_entry current_entry = NULL;
	int assigned_parameters;

	switch (current_token->kind){
	case ID:
		current_entry = find(currentTable,current_token->lexeme);
		if (!current_entry) {
			fprintf(semantic_output, "ERROR: The function 's%' is in use, but was not declared. Line number: %d \n",current_entry->name,current_token->lineNumber);
		}
		match(LPAREN);
		assigned_parameters = Parse_PARAMETERS_LIST();
		if (current_entry) {
			if (current_entry->num_of_params!=assigned_parameters) {
				fprintf(semantic_output, "ERROR: The number of parameters used in function call 's%' does not match to the function's definition. Line number: %d \n",current_entry->name,current_token->lineNumber);
			}
		}
		match(RPAREN);
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"\r\nActual token '%s',lexeme:'%s'.",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		break;
	}
}

//25
int Parse_PARAMETERS_LIST(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {RPAREN};
	int num_of_params, is_parameter =1, is_used= 1;
	//table_entry temp_entry=NULL;
	switch(current_token->kind){
	case ID:
		fprintf(syntactic_output,"Rule (PARAMETERS_LIST --> VARIABLES_LIST)\n");
		current_token=back_token();
		num_of_params = Parse_VARIABLES_LIST(integer,is_parameter,is_used);
		break;
	case RPAREN:
		fprintf(syntactic_output,"Rule (PARAMETERS_LIST --> null)\n");
		current_token=back_token();
		num_of_params = 0;
		break;
	default:
		fprintf(syntactic_output,"Expected: one of tokens 'ID', 'RPAREN' at line: %d \n",current_token->lineNumber);
		fprintf(syntactic_output,"Actual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
		ErrorHandler(follow_array, 1);
		num_of_params = 0;
		break;
	}

	return num_of_params;
}

//26
elm_type Parse_EXPRESSION(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	elm_type expression_type=error_type, right_side_type, left_side_type;
	table_entry current_entry=NULL;

	switch (current_token->kind){
		case ID:
			fprintf(syntactic_output,"Rule (EXPRESSION --> id EXPRESSION_A)\n");
			current_entry=find(currentTable,current_token->lexeme);
			if(!current_entry){
				fprintf(semantic_output,"ERROR: The variable '%s' is in use, but was not declared. Line number: %d\n",current_token->lexeme, current_token->lineNumber);
			}
			else {
				current_entry->is_used=1;
				left_side_type = current_entry->var_type;
			}
			
			right_side_type = Parse_EXPRESSION_A();
			if (right_side_type==error_type||left_side_type==error_type){
				expression_type = error_type;
			}
			else if (right_side_type==real||left_side_type==real){
				expression_type = real;
			}
			else {
				expression_type = integer;
			}
			break;
		case INTEGER:
			fprintf(syntactic_output,"Rule (EXPRESSION --> int_number)\n");
			expression_type = integer;
			break;
		case REAL_NUMBER:
			fprintf(syntactic_output,"Rule (EXPRESSION --> real_number)\n");
			expression_type = real;
			break;
		default:
			fprintf(syntactic_output,"Expected: one of tokens 'ID', 'INTEGER', 'REAL_NUMBER' at line: %d \n",current_token->lineNumber);
			fprintf(syntactic_output,"Actual token '%s',lexeme:'%s'. \n",kindToStringConverter(current_token->kind),current_token->lexeme);
			ErrorHandler(follow_array, 1);
			break;
	}

	return expression_type;
}

//27
elm_type Parse_EXPRESSION_A(){
	Token *current_token = next_token();
	eTOKENS follow_array[1] = {SEMICOLON};
	elm_type expression_type=integer;//integer - for empty cases - we don't want it to change the type that will be calculated in Parse_EXPRESSION()
	
	switch (current_token->kind){
		case LBRACKET:
			fprintf(syntactic_output,"Rule (EXPRESSION_A --> VARIABLE_A)\n");
			current_token = back_token();
			Parse_VARIABLE_A();
			break;
		case DIVISION:
		case MULTIPLICATION:
			fprintf(syntactic_output,"Rule (EXPRESSION_A --> ar_op EXPRESSION)\n");
			expression_type = Parse_EXPRESSION();
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

		return expression_type;
	}
}

//28
void print_entries() {
    entry *s, *tmp;

	HASH_ITER(hh, currentTable->data, s, tmp) {
    printf("entry name %s\n", s->name);
    /* ... it is safe to delete and free s here */
	}
}

//29
void check_if_all_variables_are_used(table_ptr current_scope){
	table_entry iterator, tmp, original;
	//Token *current_token = next_token();
	//current_token=back_token();

	HASH_ITER(hh, currentTable->data, iterator, tmp) {
		if (iterator->is_used==0){
			if (iterator->is_function==0){
				fprintf(semantic_output,"Warning: The variable '%s' is declared but is not used. Line number: %d \n",iterator->name,iterator->line_number);
				original = find(currentTable->next_table,iterator->name);
				if (original){
					original->is_used=1;
				}
				
			}
		}
	}
}

const char *typeToStringConverter(type num){
	char* valid_tokens[] = { "integer","real","error_type","Void"};

	return valid_tokens[num];
}

void add_parameters_to_function(table_ptr current_scope){
	table_ptr father = current_scope->next_table, son = current_scope;
	table_entry father_data = father->data, son_data = son->data,
	iterator, tmp, copy_entry;

	HASH_ITER(hh, father_data, iterator, tmp) {
		if (iterator->is_parameter==1){
			copy_entry = insert(son,iterator->name);
			if (copy_entry) {
				copy_entry->is_function = iterator->is_function;
				copy_entry->is_parameter = iterator ->is_parameter;
				copy_entry->is_used = iterator->is_used;
				copy_entry->line_number = iterator->line_number;
				copy_entry->name = iterator->name;
				//copy_entry->num_of_params = iterator->num_of_params;
				//copy_entry->parameters_list=iterator->parameters_list;
				//copy_entry->size = iterator->size;
				copy_entry->var_type=iterator->var_type;
			}
		}
	}
}


void add_previous_functions_entries_to_sun_table(table_ptr current_scope) {
	table_ptr father = current_scope->next_table;
	table_entry func_entry, temp_entry_copy;
	table_entry iterator, tmp, result;
	
	if (father&&father->data){
		HASH_ITER(hh, father->data, iterator, tmp) {
			if (iterator->is_function==1){
				temp_entry_copy = insert(current_scope, iterator->name);
				if (temp_entry_copy){
					temp_entry_copy->is_function = iterator->is_function;
					temp_entry_copy->is_used = iterator->is_used;
					temp_entry_copy->name = iterator->name;
					temp_entry_copy->line_number = iterator->line_number;
					temp_entry_copy->var_type = iterator->var_type; 
					temp_entry_copy->num_of_params = iterator->num_of_params;
					temp_entry_copy->return_appeared = iterator ->return_appeared;
				}		
			}
		}
	}
}


table_entry find_function_entry(table_ptr current_scope) {
	table_entry iterator, tmp, result=NULL;
	
	//for(iterator=current_scope->data; iterator != NULL; iterator->hh.next)
	HASH_ITER(hh, current_scope->data, iterator, tmp) {
		if (iterator->is_function==1 && iterator->return_appeared==0){
			result = iterator;
			break;
		}
	}

	if (result) {
		return result;
	}
	else {
		return NULL;
	}
}

void check_if_return_appeared_in_block(table_ptr current_scope) {
	table_entry iterator, tmp, result, original_entry;

	HASH_ITER(hh, current_scope->data, iterator, tmp) {
		if (iterator->is_function==1 && iterator->return_appeared==0 && iterator->var_type!=Void){
			fprintf(semantic_output, "ERROR: Return statement did not appear in function: '%s', which has type different than void. Function declared in line number: %d \n", iterator->name, iterator->line_number);
		}
		iterator->return_appeared=1;
		original_entry=find(current_scope->next_table,iterator->name);
		if (original_entry)	{
			original_entry->return_appeared=1;
		}
	}
}