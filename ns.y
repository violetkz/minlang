%{
#include "ns_ast.h"
#include <stdio.h>
#include <stdarg.h>

int yylex(void); 
void yyerror(const char *fmt, ...);

#define YYDEBUG  1
%}

%union {
    char *strval;
    int   intval;
    char *fn;
    node            *ast;
    builtin_func_node   *ast_func;
    def_func_node       *ast_def_func;
    assign_node         *ast_assign;
    assign_array_elem_node *ast_assign_array_elem;
    exp_list_node       *ast_explist;
    rule_list_node      *ast_rules;
    rule_node           *ast_rule;
    stmt_list_node      *ast_stmt_list;
    stmt_if_node        *ast_stmt_if;
    stmt_while_node     *ast_stmt_while;
    stmt_for_in_node    *ast_stmt_if_in;
    array_def_node      *ast_array_def;
    array_ref_node      *ast_array_ref;
    variable_node       *ast_variable;
    identifier_list_node      *ast_identifier_list;
    dot_call_method_node      *ast_dot_call_method;
    stmt_return_node          *ast_return_node;
};

%token  AND OR FOR IN CMP_GT CMP_LS CMP_EQ CMP_LE CMP_GE CMP_NE 
%token  IF ELSE WHILE FUNC_DEF
%token  MAIN RETURN BREAK CONTINUE

%token <strval> STR REGEXSTR  IDENTIFIER
%token <fn>     BUILTIN_FUNC
%token <intval> NUM_INT

%type <ast>  pattern stmt exp binary_operator_exp binary_compare_exp primary_exp 
             array_ref return_exp 
%type <ast_func>    func_exp  
%type <ast_def_func> def_func_exp 
%type <ast_assign>  assign_exp 
%type <ast_explist> exp_list
%type <ast_rules>   rule_list
%type <ast_rule>    rule
%type <ast_stmt_list>  stmt_list
%type <ast_identifier_list> identifier_list
%type <ast_dot_call_method> dot_call_method_exp
%type <ast_assign_array_elem> assign_array_elem_exp
%type <ast_variable> variable
%type <ast_return_node>  stmt_return_exp


/* Lowest to highest */
%right '='
%left OR AND
%nonassoc CMP_GE CMP_LE CMP_LS CMP_EQ CMP_GT CMP_NE
%left '+' '-'
%left '*' '/' '%'

%start start
%%
start: program
     ;

/*
start: rule_list { if ($1 != NULL) {
                    $1->eval();
                    }
                 }
    | MAIN '{' stmt_list '}'
                { 
                if ($3 != NULL) $3->eval();
                }

    ;
*/

program: def_func_list main
    ;

main: MAIN '{' stmt_list '}' { if ($3 != NULL) $3->eval(); }
    ;

def_func_list:  /* empty */
    | def_func_list def_func_exp 
 

rule_list: /* empty */ { $$ = NULL; } 
    | rule_list rule   { 
                            $$ = $1;
                            if ($$ == NULL) {
                                $$ = new rule_list_node;
                            }
                            $$->append($2);
                       }
    ;
    
rule: pattern '{' stmt_list '}' { $$ = new rule_node($1, $3); }
    ;

pattern: /* empty */ {$$ = NULL;}
    | REGEXSTR  { $$ = new regex_str_node($1);}
    | STR       { $$ = new str_node($1);}
    ;


stmt:    FOR variable IN variable '{' stmt_list '}'  
                { $$ = new stmt_for_in_node($2, $4, $6); }
        | WHILE '(' exp ')' '{' stmt_list '}'
                { $$ = new stmt_while_node($3, $6); }
        | IF '(' exp ')' '{' stmt_list '}'
                { $$ = new stmt_if_node($3, $6); }
        | IF '(' exp ')' '{' stmt_list '}' ELSE '{' stmt_list '}'
                { $$ = new stmt_if_node($3, $6, $10); }
        | exp ';'           { $$ = $1; }
        | BREAK ';'     { $$ = new stmt_break_node; }
        | CONTINUE ';'  { $$ = new stmt_continue_node; }
        ;

stmt_list: /* empty */    { $$ = NULL; }
         | stmt_list stmt {
                            $$  = $1;
                            if ($$ == NULL) {
                                $$ = new stmt_list_node;
                            }

                            $$->append($2);
                           }
        ;

/* expression */
exp: binary_operator_exp
   | binary_compare_exp
   | primary_exp
   | assign_exp {$$ = $1;}
   | assign_array_elem_exp {$$ = $1;}
   | array_ref  {$$ = $1;}       
   | dot_call_method_exp {$$=$1;}
   | stmt_return_exp { $$ = $1; }
   ;

/* +, -, *, /, operator */
binary_operator_exp:
      exp '+' exp       { $$ = new operator_node('+', $1, $3); } 
    | exp '-' exp       { $$ = new operator_node('-', $1, $3); }
    | exp '*' exp       { $$ = new operator_node('*', $1, $3); }
    | exp '/' exp       { $$ = new operator_node('/', $1, $3); }
    | exp '%' exp       { $$ = new operator_node('%', $1, $3); }
    ;

/* comparation expression */
binary_compare_exp:
      exp CMP_GT exp    { $$ = new compare_node(CMP_GT, $1, $3); }
    | exp CMP_LS exp    { $$ = new compare_node(CMP_LS, $1, $3); }
    | exp CMP_EQ exp    { $$ = new compare_node(CMP_EQ, $1, $3); }
    | exp CMP_NE exp    { $$ = new compare_node(CMP_NE, $1, $3); }
    | exp CMP_LE exp    { $$ = new compare_node(CMP_LE, $1, $3); }
    | exp CMP_GE exp    { $$ = new compare_node(CMP_GE, $1, $3); }
    | exp AND    exp    { $$ = new compare_node(AND, $1, $3);    }
    | exp OR     exp    { $$ = new compare_node(OR, $1, $3);     }
    ;
    
primary_exp:
     '(' exp ')'        { $$ = $2; }
    | STR               { $$ = new str_node($1);               }
    | REGEXSTR          { $$ = new regex_str_node($1);         }
    | NUM_INT           { $$ = new int_node($1);               }
    | '[' exp_list ']'  { $$ = new array_def_node($2);         }
    | func_exp          { $$ = $1; }
    | variable          { $$ = $1; }
    ;

variable: IDENTIFIER { $$ = new variable_node($1); }

array_ref:
    primary_exp '[' exp ']' { $$ = new array_ref_node($1, $3);}
    ;

dot_call_method_exp:
    primary_exp '.' IDENTIFIER '(' exp_list ')'  
        {
            $$ = new dot_call_method_node($1, $3, $5);
        } 
    ;
    

exp_list: /* empty */ {$$ = NULL;}
    | exp             
                    { 
                    $$ = new exp_list_node;
                    $$->append($1);
                    }
    | exp_list ',' exp 
                    { 
                    $$  = $1;
                    if ($$ == NULL) {
                        $$ = new exp_list_node;
                    }

                    $$->append($3);
                    }
    ;

/* func_exp: BUILTIN_FUNC '(' exp_list ')'  */
func_exp: IDENTIFIER '(' exp_list ')'
        { $$ = new builtin_func_node($1, $3); }
    ;

def_func_exp: FUNC_DEF IDENTIFIER '(' identifier_list ')' '{' stmt_list '}'
       { 
            def_func_node *n = new def_func_node($2, $4, $7); 
            symbol *s = check_symbol($2, NULL);
            if (s) {
                s->value = ns_value(n);
            }

            $$ = n; 
        }
    ;

identifier_list:  /* empty */ { $$ = NULL; }
    | IDENTIFIER           
      {
        $$ = new identifier_list_node;
        //$$->append($1)
        $$->push_back($1);
      }

    | identifier_list ',' IDENTIFIER
      { 
        $$  = $1;
        if ($$ == NULL) {
            $$ = new identifier_list_node;
        }
        //$$->append($3);
        $$->push_back($3);
      }
    ;

assign_exp: variable '=' exp  
    { $$ = new assign_node($1, $3);}
    ; 

assign_array_elem_exp: primary_exp '[' exp ']' '=' exp     
    { $$ = new assign_array_elem_node($1,$3,$6); }
    ;

return_exp: /* empty */ { $$ = NULL; }
          | exp         { $$ = $1;   }
          ;

stmt_return_exp: RETURN return_exp { $$ = new stmt_return_node($2); }
               ;

%%

void yyerror(const char *fmt, ...){

    extern int yylineno;
    extern char * yytext;
    const int MAX_MSG_LEN = 2048;

    va_list ap;
    va_start(ap, fmt);

    char msg[MAX_MSG_LEN];
    vsnprintf(msg, MAX_MSG_LEN, fmt, ap);

    printf("* Error *, Line: %d, Token:[%s]. msg: %s \n", yylineno, yytext, msg);
    va_end(ap);
}

