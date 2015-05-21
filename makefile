CPPFLAGS=-g -std=gnu++11 
CC=g++

all: ns

##ns: ns.y ns.l ns_ast.h ns_ast.cpp
#	bison -t -d ns.y 
#	flex -o ns_lex.cpp ns.l
#	
#	#mv ns.tab.c ns.tab.cpp
#	g++ -g -o ns ns.tab.c ns_lex.cpp ns_ast.cpp

ns: ns_symtbl.o ns.tab.o ns_lex.o ns_value.o ns_ast.o minlang.o
	g++ ${CPPFLAGS} -o minlang $^

minlang.o:minlang.cpp ns.tab.o

ns_ast.o:ns_ast.cpp ns_ast.h

ns.tab.o:ns.y
	bison -t -d ns.y 
	g++ -c -o ns.tab.o ${CPPFLAGS} ns.tab.c

ns_lex.o:ns.l
	flex -o ns_lex.cpp ns.l
	#g++ ${CPPFLAGS} -Wno-deprecated-register -c -o ns_lex.o ns_lex.cpp
	g++ ${CPPFLAGS}  -c -o ns_lex.o ns_lex.cpp

ns_symtbl.o:ns_symtbl.h ns_symtbl.cpp
	g++ ${CPPFLAGS} -c -o ns_symtbl.o ns_symtbl.cpp
	
ut_value: ut_ns_value.o ns_value.o
	g++ -g -o ut_value $^ 

ut_nodelist:ns_value.o ns_ast.o ut_nodelist.o
	g++ -g -o ut_nodelist $^ 

ns_value.o: ns_value.h ns_value.cpp

ut_ns_value.o: ut_ns_value.cpp

ut_nodelist.o: ut_nodelist.cpp

clean:
	rm -rf *.o *.exe  *.out *.dSYM ns lextest ns.tab.c ns.tab.cpp ns.tab.h ns_lex.c ns_lex.cpp *.stackdump ut_value ut_nodelist 

