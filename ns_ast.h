#ifndef NS_DEF_H____
#define NS_DEF_H____

#include <stdio.h>
#include <list>
#include <string>

#include "ns_value.h"
#include "ns_symtbl.h"

struct symbol;

/* the base class to present AST node */
class node {
public:
    enum {
        RULE_NODE,
        RULE_LIST_NODE,
        FUNC_PARAM_LIST_NODE,
        FUNC_PARAM_NODE,
        FUNC_NODE,
        DEF_FUNC_NODE,
        DOT_CALL_METHOD_NODE,
        ASSIGN_NODE, 
        ASSIGN_ARRAY_REF_NODE,
        STR_NODE,
        REGEX_STR_NODE,
        EXP_NODE, 
        EXPLIST_NODE,
        IDENTIFIER_NODE,
        IDENTIFIER_LIST_NODE,
        NUM_INT_NODE,
        OPERATOR_NODE,
        COMPARE_NODE,
        ARRAY_DEF_NODE,
        ARRAY_REF_NODE,
        STMT_RETURN_NODE,
        STMT_WHILE_NODE,
        STMT_IF_NODE,
        STMT_LIST_NODE,
        STMT_FOR_IN_NODE,
        STMT_BREAK_NODE,
        STMT_CONTINUE_NODE
    };
    
    node(int t) : type(t) {}
    virtual ~node() {}

    /* eval */
    virtual ns_value eval(ns_rt_context *rtctx = NULL) {
        return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);
    }
    
public:
    int type;
};

class variable_node : public node {
public:
    variable_node(char *s) : node(IDENTIFIER_NODE), id(s) {}
    
    ns_value set_value(ns_rt_context *rtctx, ns_value v);
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    //symbol *sym;
    std::string id;
};

/* string node */
class int_node : public node {
public:
    int_node(int n) : node(NUM_INT_NODE), i(n) {
        /* do nothing */
    }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    int i;
};

/* string node */
class str_node : public node {
public:
    str_node(char *s) : node(STR_NODE), str(s) {
        /* do nothing */
    }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    char *str;
};

class regex_str_node : public node {
public:
    regex_str_node(char *str)
        :node(REGEX_STR_NODE), regex_str(str) {
        /* do nothing */  
    }
public:
    char *regex_str;
};

template<typename T, int NS_NODE_TYPE> 
class node_list : public node 
{
public:
    typedef typename std::list<T*>::iterator nl_iter;
    node_list(): node(NS_NODE_TYPE) {}

    inline void append(T* n) { nlist.push_back(n); }
    inline nl_iter begin() { return nlist.begin(); }
    inline nl_iter end() { return nlist.end(); }
    
private:
    std::list<T*> nlist;
};

typedef std::list<const char *> identifier_list_node;

typedef node_list<node, node::EXPLIST_NODE> explist_base;
class exp_list_node : public  explist_base {
public:
    exp_list_node() : explist_base() {}
    ns_value eval(ns_rt_context *rtctx = NULL);
};

class def_func_node : public node {
public:
    def_func_node(char *name, identifier_list_node *args, node *stmts);
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    char                 *func_name;
    node                 *stmt_list;
    identifier_list_node *arg_list;
};

class array_def_node : public node {
public:
    array_def_node(exp_list_node* elems)
        : node(ARRAY_DEF_NODE), elements(elems) {
    }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    exp_list_node *elements;
};

class array_ref_node : public node {
public:
    array_ref_node(node *pexp, node *idx_exp)
        : node(ARRAY_REF_NODE), postfix(pexp), index(idx_exp) {
    }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    node *postfix;
    node *index;
};

class assign_array_elem_node : public node {
public:
    assign_array_elem_node(node *p, node *idx, node *v) 
        :node(ASSIGN_ARRAY_REF_NODE),
        postfix(p), index(idx), rvalue(v) {
        }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    node *postfix;
    node *index;
    node *rvalue;
};

class dot_call_method_node : public node {
public:
    dot_call_method_node(node *pexp, char *func_name, node *arglist)
        : node(DOT_CALL_METHOD_NODE), 
        postfix(pexp),
        name(func_name),
        args(arglist) {
     }
    ns_value eval(ns_rt_context *rtctx = NULL) {return ns_value(NSVAL_STATUS, NSVAL_STATUS_OK);}
public:
    node    *postfix;
    //symbol  *name;
    char    *name;
    node    *args;
};

class rule_node : public node {
public:
    rule_node(node *p, node *act) 
            :node(RULE_NODE),
            pattern(p), 
            action(act) {
        /* do nothing */
    }
    ns_value eval(ns_rt_context *rtctx = NULL); 

public:
    node *pattern;
    node *action;
};

typedef node_list<rule_node, node::RULE_LIST_NODE> rule_list_base;
class rule_list_node : public rule_list_base {
public:
    rule_list_node() : rule_list_base() {}
    ns_value eval(ns_rt_context *rtctx = NULL);
};


typedef node_list<node, node::STMT_LIST_NODE> stmt_list_base;
class stmt_list_node : public stmt_list_base {
public:
    stmt_list_node() : stmt_list_base() {}
    ns_value eval(ns_rt_context *rtctx = NULL);
};

class assign_node : public node {
public:
    assign_node(variable_node *id, node *val)
            : node(ASSIGN_NODE),
             variable(id), 
             rvalue(val) {
        /* do nothing */
    }
    ns_value eval(ns_rt_context *rtctx = NULL);

public:
    variable_node *variable;
    node *rvalue;
};

class stmt_while_node : public node {
public:
    stmt_while_node(node *condition, stmt_list_node *stmt_list) 
        :node(STMT_WHILE_NODE), condition_exp(condition), stmts(stmt_list) {
    }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    node *condition_exp;
    stmt_list_node *stmts;
};

class stmt_if_node : public node {
public:
    stmt_if_node(node *condition, stmt_list_node *action) 
        : node(STMT_IF_NODE), condition_exp(condition), 
        stmts(action), else_stmts(NULL) {
     }

    stmt_if_node(node *condition, stmt_list_node *action,
            stmt_list_node *else_action)
        : node(STMT_IF_NODE), condition_exp(condition),
        stmts(action), else_stmts(else_action) {
     }

    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    node *condition_exp;
    stmt_list_node *stmts;
    stmt_list_node *else_stmts;
};

class stmt_for_in_node : public node {
public:
    stmt_for_in_node(variable_node *tmp, variable_node *ln, stmt_list_node *stmt_list)
        : node(STMT_FOR_IN_NODE), tmp_id(tmp), id(ln), stmts(stmt_list) {
    }
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    variable_node  *tmp_id;
    variable_node  *id; 
    stmt_list_node  *stmts;
};

class operator_node : public node {
public:
    operator_node(char opt, node *l, node *r) 
        : node(OPERATOR_NODE), opt(opt), left(l), right(r) {
    }
    
    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    char    opt; 
    node  *left;
    node  *right;
};

class compare_node : public node {
public:
    compare_node(int opt, node *l, node *r) 
        : node(COMPARE_NODE), cmp_opt(opt), left(l), right(r) {
    }

    ns_value eval(ns_rt_context *rtctx = NULL);
public:
    int    cmp_opt; 
    node  *left;
    node  *right;
};


class builtin_func_node : public node {
public:
    builtin_func_node(const char *name, exp_list_node *plist)
                    :node(FUNC_NODE),
                    func_name(name),
                    plist(plist) {
        /* do nothing */
    }
    
    ns_value eval(ns_rt_context *rtctx = NULL);

public:
    const char *func_name;
    exp_list_node *plist;
};

class stmt_return_node : public node {
public:
    stmt_return_node(node *exp);
    ns_value eval(ns_rt_context *rtctx = NULL);
private:
    node *retval_exp;
};

class stmt_break_node : public node {
public:
    stmt_break_node():node(STMT_BREAK_NODE) {}
    ns_value eval(ns_rt_context *rtctx = NULL) {
        return ns_value(NSVAL_STATUS, NSVAL_STATUS_BREAK);
    };
};

class stmt_continue_node : public node {
public:
    stmt_continue_node():node(STMT_CONTINUE_NODE) {}
    ns_value eval(ns_rt_context *rtctx = NULL) {
        return ns_value(NSVAL_STATUS, NSVAL_STATUS_CONTINUE);
    };
};

#endif //~NS_DEF_H____
