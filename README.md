# compilers-project
Final project - "Theory of Compilation" course in HIT (2019) - educational project

Introduction:
This project simulates a compiler for fictional programming language "G" made up for educational purposes.
This language, like any other programming language has it's own rules, keywords and other features - all these are referred as the language's grammer.
The compilre we built in this project gets 2 text files, and parses them as if they were code in "G" language.
It then creates 3 output files for each of the analyzers a compiler should have - lexical, sytactic and semantic (described bellow).

The project consists of 3 parts:
1. Lexical analyzer - responsible for finding tokens that are part of the programming language.
2. Syntactic analyzer - responsible for verifying that the structure of the code is legal and fits the programming language grammer
3. Semantic analyzer - responsible for finding logical errors in the code. Finds errors the the analyzers above can't find.

The code represented in here has already all 3 parts.
Language's grammer is uploaded to this folder as well.

Test files and outputs should be placed under "C:/temp" in the user's PC.

External dependencies:
1. Lexical analyzer contains list of reserved keywords and regular expresions and maps them with a list of actions that should be performed when we have a match to any of the keywords/expressions.
For doing so, the lexical analyzer uses a program called FLEX - which takes the list of keywords and regular expressions and converts them to C code with the corresponding actions that should be performed when a match is found
2. The semantic analyzer requires use of hash tables, in order to store and fetch the tokens that are used in each part (scope) of the program.
I used the "UThash" library which gives a rather convinient API for performing hash on complex structures in C.
