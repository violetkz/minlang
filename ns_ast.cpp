#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <iostream>
#include <iterator>
#include "ns_ast.h"
#include "ns_symtbl.h"
#include "ns.tab.h"

extern void free_strval(char*);

ns_value variable_node::set_value(ns_rt_context *rtctx, ns_value v) {
    symbol *sym = check_symbol(id, rtctx);
    if (sym) { 
        sym->value = v; 
    }
    return sym->value;
}

ns_value variable_node::eval(ns_rt_context *rtctx) {
    symbol *sym = check_symbol(id, rtctx);
    if (!sym) { //xxx
    }
    return sym->value;
}

ns_value int_node::eval(ns_rt_context *rtctx) {
    return ns_value(i);
}

ns_value str_node::eval(ns_rt_context *rtctx) {
    return ns_value(str); 
}

ns_value rule_node::eval(ns_rt_context *rtctx) {
    pattern->eval(rtctx);
    action->eval(rtctx);
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
}

ns_value rule_list_node::eval(ns_rt_context *rtctx) {
    nl_iter it = begin();
    for (;it != end(); ++it) {
        (*it)->eval(rtctx);
    }
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
}

ns_value exp_list_node::eval(ns_rt_context *rtctx) {

    nl_iter it = begin();
    for (;it != end(); ++it) {
        (*it)->eval(rtctx);
    }
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
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
            set_elem(pv, idx.int_val, rvalue->eval(rtctx));  
        }
        return pv;
    }
    return ns_value(NSVAL_ILLEGAL);
}

ns_value builtin_func_node::eval(ns_rt_context *rtctx) {

    // eval paramter list
    std::list<ns_value> func_param_value_list;
    exp_list_node::nl_iter it = plist->begin();
    for(; it != plist->end(); ++it) {
        func_param_value_list.push_back((*it)->eval(rtctx));
    }

    if (strcmp(func_name, "print") == 0) {

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
                    //create a func run time environment.
                    ns_rt_context func_rt_ctx;  
                    //pass the paramter list
                    func_rt_ctx.func_param_list = &func_param_value_list;
                    //eval the func node under local env;
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
    // TODO  
    return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
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
    exp_list_node::nl_iter it = elements->begin();
    ns_value v_list(NSVAL_LIST);
    for (; it != elements->end(); ++it) {
        ns_value v = (*it)->eval(rtctx);     
        if (! v.is_illegal_value()) {
            v_list.list_val->push_back(v);
        }
    }
    return v_list;
}

ns_value array_ref_node::eval(ns_rt_context *rtctx) {
    ns_value pv = postfix->eval(rtctx);
    if (pv.is_iteratale()) {
        ns_value idx = index->eval(rtctx);        
        if (idx.is_int()) {
            return get_elem(pv, idx.int_val);            
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
    if (rtctx->func_param_list->size() == arg_list->size()) {
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
