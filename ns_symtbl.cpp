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
#include <list>
#include <algorithm>

#include "ns_symtbl.h"
#include "ns_util.h"

typedef std::list<char *> str_tbl;

struct global_env {
    symtbl  global_symtbl;
    str_tbl global_strtbl;
};


class ns_global_env {
public:
    static global_env *get_env() {
        if (rt_env == NULL) {
            rt_env = new global_env;
        }
        return rt_env;
    }

    static symtbl &get_tbl() {
        global_env *env = get_env();
        return env->global_symtbl; 
    }
    
    static str_tbl &get_strtbl() {
        global_env *env = get_env();
        return env->global_strtbl;
    }

private:
    static global_env  *rt_env;
};

global_env *ns_global_env::rt_env = NULL;

/* install a symbol into table */
symbol *check_symbol(const std::string& name, ns_rt_context *rt) {

    symtbl &tbl = (rt) ? rt->local_env : ns_global_env::get_tbl();

    symtbl_iterator it = tbl.find(name);

    /* if not existed, create new item */
    symbol *n = NULL;
    if (it == tbl.end()) { 
        n = new symbol;
        n->id = name;
        tbl[name] = n;
    }
    else {
        n = tbl[name];
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
    symtbl &tbl = ns_global_env::get_tbl();
    symtbl_iterator it = tbl.find(name);

    if (it != tbl.end()) {
        re = tbl[name];
    }
    return re;
}

void put_str_to_pool(char *s) {
    str_tbl &strtbl = ns_global_env::get_strtbl();
    strtbl.push_back(s);
}

void clean_str_pool() {
    str_tbl &strtbl = ns_global_env::get_strtbl();
    std::for_each(strtbl.begin(), strtbl.end(), 
             [](char *p) { free_raw_str(p); }
            );
}
