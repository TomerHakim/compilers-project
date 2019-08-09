%{
%}
%%
.        ECHO;
%%
void main(int argc, char* argv[])
{
yyin=fopen(argv[1],"r");
 yylex();
}
