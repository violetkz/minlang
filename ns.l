%option noyywrap nodefault yylineno
%{

#include "ns_ast.h"
#include "ns.tab.h"
#include "ns_symtbl.h"

extern void yyerror(const char *s, ...);

char *makebuffer(size_t n);

%}
%%
[ \t\n]    ;   /* Skip  whitespaces */
#.*\n      ;   /* skip comments */

"if"    { return IF;  } 
"else"  { return ELSE;}
"in"    { return IN;  }
"for"   { return FOR; }
"while" { return WHILE;}
"main"  { return MAIN;}
"def"   { return FUNC_DEF;}
"return" { return RETURN;}
"break"  { return BREAK;}
"continue" {return CONTINUE;}

"and"   { return AND; }
"or"    { return OR;  }
">"     { return CMP_GT; }
"<"     { return CMP_LS; }
"=="    { return CMP_EQ; }
"!="    { return CMP_NE; }
">="    { return CMP_GE; }
"<="    { return CMP_LE; }


"len"    |
"substr" |
"split"  |
"print"  |
"getline"  {
    /* yylval.fn = strdup(yytext); return BUILTIN_FUNC;
       */
                yylval.strval = strdup(yytext);
                return IDENTIFIER; 
        }

[0-9]+     {yylval.intval = atoi(yytext); return NUM_INT;}
[A-Za-z_][A-Za-z0-9_]*  { 
                /* yylval.sym = check_symbol(yytext);  */
                yylval.strval = strdup(yytext);
                return IDENTIFIER; 
               }
r{0,1}\"[^"]*\" { 
                char *buff = makebuffer(yyleng + 1);
                char str_type = yytext[0];
                int skip_num = 0;
                if (str_type == 'r') { /* regrex str */
                    skip_num = 2;      /* skip `r"`  */ 
                }
                else {
                    skip_num = 1;      /* skip `"`  */
                }
                char *s = yytext + skip_num;
                char *d = buff;

                for (int i = 0; i < yyleng - 2; ++i) {
                    *d++ = *s++;
                }
                *d = '\0'; /* make sure a end-char */
                yylval.strval = buff;
                return STR;
                }
"." |
"[" |
"]" |
"=" |
";" |
"," |
"{" |
"}" |
")" |
"+" |
"-" |
"*" |
"/" |
"%" |
"("         {return yytext[0]; }

.           { yyerror("unkown token."); }
%%

char* makebuffer(size_t n) {
    char* buff = (char *)malloc(n);
    memset(buff, 0x00, n);
    return buff;
}

void free_strval(char *s) {
    if (s != NULL) free(s);
}
