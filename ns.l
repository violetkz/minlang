/*
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 */


%option noyywrap nodefault yylineno
%{

#include "ns_ast.h"
#include "ns.tab.h"
#include "ns_symtbl.h"
#include "ns_util.h"

extern void yyerror(const char *s, ...);

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

[0-9]+     {yylval.intval = atoi(yytext); return NUM_INT;}

[A-Za-z_][A-Za-z0-9_]*  { 
                yylval.id = strdup(yytext);
                put_str_to_pool(yylval.id);
                return IDENTIFIER; 
               }

r{0,1}\"[^"]*\" { 
                char *buff = make_raw_str_buffer(yyleng + 1);
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
                put_str_to_pool(buff);
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
