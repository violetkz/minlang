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


#ifndef ns_symtbl_hpp__
#define ns_symtbl_hpp__

#include "ns_value.h"

#include <map>
#include <string>


/* symbol info */
struct symbol {
    std::string id;
    ns_value    value;
    
};

/* symbol table */
typedef std::map<std::string, symbol*> symtbl;
typedef std::map<std::string, symbol*>::iterator symtbl_iterator;

/* the runtime context */
struct ns_rt_context {
    symtbl local_env;      // local symbol table
    std::vector<ns_value> *func_param_list; // function paramter list
    ns_value               func_return_val; // store return value of function
};

/* install a symbol into table */
symbol *check_symbol(const std::string& name, ns_rt_context *rt);

/* find a symbol from table by name */
symbol *find_symbol(const std::string& name, ns_rt_context *rt);

/* put str pointer into pool */
void put_str_to_pool(char *s);

/* clean string pool */
void clean_str_pool(); 

#endif //~ns_symtbl_hpp__
