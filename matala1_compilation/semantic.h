#ifndef	SEMANTIC
#define SEMANTIC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h"
/*
In order to manage symbol table as required, we chose to use an API we found on the internet called UTHash.
This API simply allows to perform hash on any structure we create, and gives functions to manage the hash table (such as, insert items, delete, etc..)
This makes it easier for us to manage the symbol table.

A symbol table will look as follow:
####################################
# pointer to a father symbol table #
####################################
#     data entires - hash table    #
#             entry1               #
#             entry2               #
#              ...                 #
#                                  #
####################################

The UTHash tool will help us manage the data entries, but it's our resposibility to mange the entire symbol table.
Written by: Elad and Tomer
*/

typedef enum type {
	integer,
	real,
	error_type,
	Void
}type;


typedef struct parameter{
	char* lexeme;
	type variable_type;
} parameter;


typedef struct symbol_table_data {
	/*
	The UT_hash_handle field must be present in our structure. It is used for the internal bookkeeping that makes the hash work. 
	It does not require initialization. This allows us to use the easier "convenience" macros to add, find and delete items.
	*/
	UT_hash_handle hh;
	int is_function; // flag to check if variable or function
	int is_used; // flag to check if used
	int line_number;
	//type returned_value;
	
	//relevant for variables
	char* name;
	type var_type;
	int size;
	int is_parameter;
	
	//relevant for functions
	int num_of_params;
	char* parameters_list;
	int return_appeared;
} entry;

typedef struct table_node {
	struct table_node* next_table;
	entry* data;
} table_node;

typedef table_node* table_ptr; //pointer to table
typedef entry* table_entry; //pointer to entry in the table
typedef type elm_type; //pointer to type

table_ptr make_table(table_ptr current_table);
table_ptr pop_table(table_ptr current_table);
table_entry insert(table_ptr current_table, char* id_name);
table_entry lookup(table_ptr current_table, char *id_name);
table_entry find(table_ptr current_table, char *id_name);
void set_id_type(table_entry id_entry, elm_type id_type);
elm_type get_id_type(table_entry id_entry);
void set_size(table_entry id_entry, int size);




#endif