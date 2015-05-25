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

#include <cassert>
#include <cstdio>
#include <cstring>

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>

#include "ns_ast.h"
#include "ns_symtbl.h"
#include "ns.tab.h"
#include "ns_util.h"

ns_value variable_node::set_value(ns_rt_context *rtctx, ns_value v) {
    symbol *sym = check_symbol(id, rtctx);
    if (sym) { 
        sym->value = v; 
    }
    return sym->value;
}

ns_value variable_node::eval(ns_rt_context *rtctx) {
    symbol *sym = check_symbol(id, rtctx);
    if (!sym) {
    }
    return sym->value;
}

ns_value int_node::eval(ns_rt_context *rtctx) {
    return ns_value(i);
}

ns_value str_node::eval(ns_rt_context *rtctx) {
    return ns_value(str); 
}

ns_value exp_list_node::eval(ns_rt_context *rtctx) {

    ns_value n(NSVAL_LIST);
    nl_iter it = begin();
    for (;it != end(); ++it) {
        n.append((*it)->eval(rtctx));
    }
    return n;
}

ns_value assign_node::eval(ns_rt_context *rtctx) {
    variable->set_value(rtctx, rvalue->eval(rtctx));
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
}

ns_value assign_array_elem_node::eval(ns_rt_context *rtctx) {
    ns_value pv = postfix->eval(rtctx);

    if (pv.is_iteratale()) {
        ns_value idx = index->eval(rtctx);        
        if (idx.is_int()) {
            pv.set_elem(idx.int_val, rvalue->eval(rtctx));  
        }
        return pv;
    }
    return ns_value(NSVAL_ILLEGAL);
}

ns_value builtin_func_node::eval(ns_rt_context *rtctx) {

    // eval paramter list
    std::vector<ns_value> func_param_value_list;
    exp_list_node::nl_iter it = plist->begin();
    for(; it != plist->end(); ++it) {
        func_param_value_list.push_back((*it)->eval(rtctx));
    }

    if (func_name == "print") {

        std::copy(func_param_value_list.begin(), func_param_value_list.end(), 
                std::ostream_iterator<ns_value> (std::cout));
        std::cout << std::endl;
    }
    else{
        symbol *s = find_symbol(func_name, rtctx);
        if (s) {
            ns_value ns =  s->value;
            if (ns.type == NSVAL_EXPERESS_AST) {
                node *func = ns.node_val;
                if (func) {
                    // create a func run time environment.
                    ns_rt_context func_rt_ctx;  
                    // pass the paramter list
                    func_rt_ctx.func_param_list = &func_param_value_list;
                    // eval the func node under local env;
                    ns_value func_status = func->eval(&func_rt_ctx);
                    if (func_status.is_status_return()) {
                        return func_rt_ctx.func_return_val; 
                    }
                    return func_status;
                }
            }
        }
        else {
            std::cerr << "can't find the symbol: " << func_name << std::endl;
            return ns_value(NSVAL_ILLEGAL);
        }
    }
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
}


ns_value stmt_if_node::eval(ns_rt_context *rtctx) {
    ns_value retval(NSVAL_STATUS, NSVAL_STATUS_OK);
    ns_value cond = condition_exp->eval(rtctx);
    if (cond) {
        retval = stmts->eval(rtctx);
    }
    else if (else_stmts != NULL) {
        retval = else_stmts->eval(rtctx);
    }

    return retval;
}

ns_value stmt_while_node::eval(ns_rt_context *rtctx) {
    ns_value retval(NSVAL_STATUS, NSVAL_STATUS_OK);
    while (condition_exp->eval(rtctx)) {
        retval = stmts->eval(rtctx);
        if (retval.is_status_break() || retval.is_illegal_value()) 
            break;
        else if (retval.is_status_continue()) {
            continue;
        }
    }
    return retval;
}

ns_value stmt_for_in_node::eval(ns_rt_context *rtctx) {
    tmp_id->eval(rtctx); 
    ns_value v = id->eval(rtctx);
    if (v.is_array() && v.list_val) {
        auto it = v.list_val->begin();
        bool run_status = true;
        for (; it != v.list_val->end(); ++it) {
            
            tmp_id->set_value(rtctx, *it);

            ns_value status = stmts->eval(rtctx);
            if (status.is_illegal_value()) {
                run_status = false;
                break;
            }
        }

        if (run_status) return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
        
    }
    return ns_value(NSVAL_ILLEGAL);
}

ns_value operator_node::eval(ns_rt_context *rtctx) {

    ns_value l = left->eval(rtctx);
    ns_value r = right->eval(rtctx);
    ns_value n;
    switch (opt) {
        case '+':  n = l + r; 
                   break;
        case '-':  n = l - r;
                   break;
        case '*':  n = l * r;
                   break;
        case '/':  n = l / r;
                   break;
        case '%':  n = l % r;
                   break;
        default:
                   n = ns_value(NSVAL_ILLEGAL);
                   fprintf(stderr, "* error *: un-defined operator: %c\n", opt);
                   break;
    }
    return n;
}

ns_value compare_node::eval(ns_rt_context *rtctx) {

    ns_value l = left->eval(rtctx);
    ns_value r = right->eval(rtctx);

    bool v = false;
    switch (cmp_opt) {
        case CMP_GT:
            v = l > r;
            break;
        case CMP_LS:
            v = l < r;
            break;
        case CMP_EQ:
            v = l == r;
            break;
        case CMP_NE:
            v = l != r;
            break;
        case CMP_GE:
            v = l >= r;
            break;
        case CMP_LE:
            v = l <= r;
            break;
        case OR:
            v = l || r;
            break;
        case AND:
            v = l && r;
            break;
        default:
            v = false; 
            break;
    }
    return ns_value(v);
}

ns_value stmt_list_node::eval(ns_rt_context *rtctx) {

    nl_iter it = begin();
    for (;it != end(); ++it) {
        node *n = *it;
        ns_value val = n->eval(rtctx);
        if (val.is_status_break() 
                || val.is_status_continue()
                || val.is_status_return()
                || val.is_illegal_value()) {
            return val;
        }
    }
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
}


ns_value array_def_node::eval(ns_rt_context *rtctx) {

    ns_value v = elements->eval(rtctx);
    if (v.is_array()) {
        return v;
    }

    return ns_value(NSVAL_ILLEGAL);
}

ns_value array_ref_node::eval(ns_rt_context *rtctx) {
    ns_value pv = postfix->eval(rtctx);
    if (pv.is_iteratale()) {
        ns_value idx = index->eval(rtctx);        
        if (idx.is_int()) {
            return pv.get_elem(idx.int_val);            
        }
        std::cerr 
            << "index is not integer type." << std::endl;
    }
    std::cerr
        << "the object is not iteratable" << std::endl;

    return ns_value(NSVAL_ILLEGAL);
}

def_func_node::def_func_node(char *name, identifier_list_node *args, node *stmts)
    :node(DEF_FUNC_NODE), func_name(name), arg_list(args), stmt_list(stmts) {
    }

ns_value def_func_node::eval(ns_rt_context *rtctx) {

    // push paramter stack
    if (arg_list != NULL && rtctx->func_param_list->size() == arg_list->size()) {
        auto it = arg_list->begin();
        auto pit = rtctx->func_param_list->begin();
        for (; it != arg_list->end(); ++it, ++pit) {
            symbol *s = check_symbol(*it, rtctx); 
            s->value = *pit; 
        }
    }
    ns_value val = stmt_list->eval(rtctx);
    return val;
}

/** 
 * class stmt_return_node 
 */
stmt_return_node::stmt_return_node(node *exp) : node(STMT_RETURN_NODE), retval_exp(exp) {
}

ns_value stmt_return_node::eval(ns_rt_context *rtctx) {
    ns_value retval(NSVAL_STATUS, NSVAL_STATUS_RETURN);
    if (retval_exp) {
        rtctx->func_return_val = retval_exp->eval(rtctx); 
    }
    return retval;
}

ns_value dot_call_method_node::eval(ns_rt_context *rtctx) {
    ns_value arr = postfix->eval(rtctx);

    // eval paramter list
    ns_value arglist = args->eval(rtctx);
    std::vector<ns_value> *plist = arglist.list_val;

    if (arr.is_array() && arglist.is_array()) {
        if (name == "append") {
            if (arglist.len() == 1) {
                arr.append((*plist)[0]);
                return arr;
            }
        }
        else if (name == "len") {
            return ns_value(arr.len()); 
        }
        else if (name ==  "del") {
            if (arglist.len() == 1) {
                ns_value arg = (*plist)[0];
                if (arg.is_int()) {
                    arr.del(arg.int_val);
                }
                else {
                    std::cerr << "paramter isn't type int!" << std::endl;
                }
            }
            else {
                std::cerr << "Bad paramter number!" << std::endl;
            }
            return arr;
        }
        else if (name == "find") {
            if (arglist.len() == 1) {
                int idx = arr.find((*plist)[0]);
                if (idx == -1) {
                    std::cerr << "out of index. " << std::endl;
                    return ns_value(NSVAL_ILLEGAL);
                }
                return ns_value(idx);
            }
        }
        else {
            std::cerr << "func: " << name << "doesn't support." << std::endl;
            return ns_value(NSVAL_ILLEGAL);
        }
    }
    else if (arr.is_raw_string()) {
        if (name == "len") {
            return ns_value(arr.len());
        }
    }

    return ns_value(NSVAL_ILLEGAL);
}
