#ifndef PARSER_H
#define PARSER_H
#include "Token.h"
#include "semantic.h"

/*
typedef table_node* table_ptr;	//pointer to table
typedef entry* table_entry;		//pointer to entry in the table
typedef type elm_type;			//type
*/

/*1*/void Parser();
/*2*/int match(eTOKENS t);
/*3*/void ErrorHandler(eTOKENS* follow_tokens_array, int size);
/*4*/void Parse_PROGRAM();//is_function=0;
/*5*/int Parse_VAR_DEFINITIONS(int is_parameter); //returns number of variables in list (all types)
/*6*/int Parse_VAR_DEFINITIONS_A(int is_parameter); //returns number of variables in list (all types)
/*7*/int Parse_VAR_DEFINITION(int is_parameter); //returns number of variables in list (all types)
/*8*/elm_type Parse_TYPE();
/*9*/int Parse_VARIABLES_LIST(elm_type var_type, int is_parameter, int is_used); //returns number of variables in list (single type)
/*10*/int Parse_VARIABLES_LIST_A(elm_type var_type, int is_parameter, int is_used);//returns number of variables in list (single type)
/*11*/void Parse_VARIABLE(elm_type var_type, int is_parameter, int is_used); //is_function=0
/*12*/int Parse_VARIABLE_A(); //returns array size
/*13*/void Parse_FUNC_DEFINITIONS();
/*14*/void Parse_FUNC_DEFINITIONS_A();
/*15*/void Parse_FUNC_DEFINITION();//is_function=1
/*16*/elm_type Parse_RETURNED_TYPE();
/*17*/int Parse_PARAM_DEFINITIONS();
/*18*/void Parse_STATEMENTS(elm_type func_type);
/*19*/void Parse_STATEMENTS_A(elm_type func_type);
/*20*/void Parse_STATEMENT(elm_type func_type);
/*21*/elm_type Parse_STATEMENT_A();
/*22*/void Parse_STATEMENT_B();
/*23*/void Parse_BLOCK(elm_type func_type);//is_function=0
/*24*/void Parse_FUNCTION_CALL();
/*25*/int Parse_PARAMETERS_LIST();//is_parameter=1
/*26*/elm_type Parse_EXPRESSION();
/*27*/elm_type Parse_EXPRESSION_A();
/*28*/void print_entries();
/*29*/void check_if_all_variables_are_used(table_ptr current_scope);
/*30*/const char *typeToStringConverter(elm_type num);
/*31*/void add_parameters_to_function(table_ptr current_scope);
/*32*/table_entry find_function_entry(table_ptr current_scope);
/*33*/void check_if_return_appeared_in_block(table_ptr current_scope);
/*34*/void add_previous_functions_entries_to_sun_table(table_ptr current_scope);

#endif