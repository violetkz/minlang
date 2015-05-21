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


#include <map>
#include "ns_symtbl.h"

class ns_symtbl {
public:
    static symtbl *get_tbl() {
        if (tbl == NULL) {
            tbl = new symtbl;
        }
        return tbl;
    }

private:
    static symtbl *tbl;
};

symtbl *ns_symtbl::tbl = NULL;

/* install a symbol into table */
symbol *check_symbol(const std::string& name, ns_rt_context *rt) {

    symtbl *tbl = NULL;

    if (rt) {
        tbl = &rt->local_env;
    }
    else {
        tbl = ns_symtbl::get_tbl();
    }

    symtbl_iterator it = tbl->find(name);

    /* if not existed, create new item */
    symbol *n = NULL;
    if (it == tbl->end()) { 
        n = new symbol;
        n->id = name;
        (*tbl)[name] = n;
    }
    else {
        n = (*tbl)[name];
    }
    return n;
}

/* 
 * find a symbol from table by name and return pointer of symbol if existed
 * esle return NULL 
 */ 
symbol *find_symbol(const std::string& name, ns_rt_context *rt) {
    symbol *re = NULL;

    /* search local symbol table, firstly */
    if (rt) {
        auto it = rt->local_env.find(name);
        if (it != rt->local_env.end()) {
            re = rt->local_env[name]; 
            return re;
        }
    }

    /* search global symbol tablel. */
    symtbl *tbl = ns_symtbl::get_tbl();
    symtbl_iterator it = tbl->find(name);

    if (it != tbl->end()) {
        re = (*tbl)[name];
    }
    return re;
}
