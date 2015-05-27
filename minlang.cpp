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


#include <getopt.h>
#include <cstdio>
#include <cstdlib>

#include "ns_value.h"
struct cmdline_opt {
    const char* filename;           
};

void print_usage() {
    printf(
            "\n"
            "Usage:\n"
            "$ ./ns [options] \n"
            "Options:\n"
            "    -f filename     specify file name. \n"
            "    -h                   print usage.\n"
            "\n");
}

void parse_args(int argc, char** argv,  cmdline_opt* opts) {

    int c;
    int errflg = 0;

    if (argc < 3) {
        errflg = 1;
        goto err;
    }

    extern char* optarg;
    while ((c = getopt(argc, argv, "f:h")) != -1) {
        switch (c) {
            case 'f' :
                opts->filename = optarg;
                break;
            case 'h' :
                print_usage();
                exit(0);
            default :
                errflg = 1;
        }
    }

err:
    if (errflg) {
        print_usage();
        exit (2);
    }
}

int main(int args, char *argv[]) {
  extern FILE* yyin;
  extern int yyparse (void);
  cmdline_opt opts; 
  parse_args(args, argv, &opts);
  yyin = fopen(opts.filename, "r");
  if (yyin) {
      yyparse();
	  fclose(yyin);
  }
  return 0;
}
