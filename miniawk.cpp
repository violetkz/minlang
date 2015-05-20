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
    extern char* optarg;
    while ((c = getopt(argc, argv, "f:h")) != -1)
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
