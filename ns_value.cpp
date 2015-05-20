#include "ns_value.h"
#include <cstring>
#include <iterator>
#include <algorithm>

const int ns_value::need_ref_count_bit_map =
        0
        | (1 << NSVAL_LITERAL_STR) 
        | (1 << NSVAL_LIST);

bool ns_value::is_ref_count_type(ns_value_type t) {
   bool v = (need_ref_count_bit_map & (1 << t)) ? true : false;
   return v;
}

ns_value::ns_value(ns_value_type t) : type(t), int_val(0), ref_count(0) {
    switch (type) {
        case NSVAL_UNINITIALIZED:
            break;
        case NSVAL_ILLEGAL: 
            break;
        case NSVAL_STATUS:
            int_val = NSVAL_STATUS_FAILED;
            break;
        case NSVAL_INTEGER:
            int_val = 0;
            break;
        case NSVAL_LITERAL_STR:
            chr_val = new std::string;
            ref_count = new int(1);
            break;
        case NSVAL_BOOLEAN:
            bool_val = false;
            break;
        case NSVAL_LIST:
            list_val = new std::list<ns_value>;
            ref_count = new int(1);
            break;
        default:
            break;
    }
}

void ns_value::destruct() {
    if (is_ref_count_type(type) && ref_count != NULL) {
        if (*ref_count == 1) {
            switch (type) {
                case NSVAL_LITERAL_STR:
                    if (chr_val != NULL) {
                        delete chr_val;
                        chr_val = NULL;
                    }
                    break;
                case NSVAL_LIST:
                    if (list_val != NULL) {
                        delete list_val;
                        list_val = NULL;
                    }
                    break;
                default:
                    break;
            } 

            delete ref_count;
            ref_count = NULL;
        }
    }
}

void ns_value::release() { 
    if (is_ref_count_type(type) && ref_count != NULL) {
        -- (*ref_count);
    }
}

void ns_value::add_ref() { 
    if (is_ref_count_type(type) && ref_count != NULL) {
        (*ref_count)++;
    }
}

ns_value::~ns_value() {
    destruct();
}

ns_value::ns_value(const char *s) 
    : type(NSVAL_LITERAL_STR), chr_val(NULL), ref_count(0) {

    if (s != NULL) {
        chr_val = new std::string(s); 
    }

    ref_count = new int(1);
}

ns_value::ns_value(const ns_value &s)
    : type(s.type), ref_count(s.ref_count), int_val(0) {

    add_ref();
    if (type == NSVAL_LITERAL_STR) {
        chr_val  = s.chr_val;
    }
    else if (type == NSVAL_INTEGER)  int_val  = s.int_val;
    else if (type == NSVAL_STATUS)   int_val  = s.int_val;
    else if (type == NSVAL_BOOLEAN)  bool_val = s.bool_val;
    else if (type == NSVAL_LIST)     list_val = s.list_val;
    else if (type == NSVAL_EXPERESS_AST) node_val = s.node_val;
    else {
        std::cerr << "Warning! the constractor can't handle un-defined type" 
                << std::endl;
        int_val = 0;
    }
}

ns_value &ns_value::operator = (const ns_value &s) {

    if (&s == this) return *this;

    release(); //releae old;

    type      = s.type;
    ref_count =  s.ref_count;

    add_ref(); //update new
    if (s.type == NSVAL_LITERAL_STR) { chr_val = s.chr_val; }
    else if (type == NSVAL_INTEGER)  int_val = s.int_val;
    else if (type == NSVAL_STATUS)   int_val  = s.int_val;
    else if (type == NSVAL_BOOLEAN)  bool_val = s.bool_val;
    else if (type == NSVAL_LIST)     list_val = s.list_val;
    else if (type == NSVAL_EXPERESS_AST) node_val = s.node_val;
    else {
        std::cerr << "Warning! the constractor can't handle un-defined type" 
                << std::endl;
        int_val = 0;
    }
    return *this;
}

ns_value::operator bool() {
    bool v = false;
    switch (type) {
        case NSVAL_BOOLEAN:
            v = bool_val;
            break;
        case NSVAL_INTEGER:
            /* NOTE: if value is not equal with 0, return true */ 
            v = (int_val != 0) ? true : false;
            break;
        case NSVAL_LITERAL_STR:
            /* NOTE: if the string is not empty, return true */
            v = (chr_val->size() > 0) ? true : false;
            break;
        default:
            std::cerr 
                << "warning. can't convert to boolean from type:" 
                << *this << std::endl;
            break; 
    }
    return v;
}

std::ostream &operator << (std::ostream &out, const ns_value &v) {
    switch (v.type) {
        case NSVAL_UNINITIALIZED:
            out << "uninitialized value" ;
            break;
        case NSVAL_ILLEGAL:
            out << "illegal value";
            break;
        case NSVAL_STATUS:
            out << "status:" << v.int_val;
        case NSVAL_INTEGER:
            out << v.int_val;
            break;
        case NSVAL_LITERAL_STR:
            out <<  *v.chr_val ;
            break;
        case NSVAL_BOOLEAN:
            out << v.bool_val;
            break;
        case NSVAL_LIST:
            out << '[';
            if (v.list_val != NULL) {
                std::copy(v.list_val->begin(),
                          v.list_val->end(), 
                          std::ostream_iterator<ns_value>(out, ","));
            }
            out << ']';
            break;
        case NSVAL_CHAR:
            //out << chr_val;
            break;
        case NSVAL_EXPERESS_AST:
            out << "expression: " << v.node_val;
            break;
        default:
            out << "unkown type" << v.type;
            break;
    }
    return out;
}

ns_value operator+ (const ns_value &l, const ns_value &r) {
    if (l.type == r.type) {
        if (l.type == NSVAL_INTEGER) {
            return ns_value(l.int_val + r.int_val);
        }
        else if (l.type == NSVAL_LITERAL_STR) {
            //XXX
            std::string tmp = *l.chr_val + *r.chr_val;
            return  ns_value(tmp.c_str());
        }
        return ns_value(NSVAL_ILLEGAL);
    }
    return ns_value(NSVAL_ILLEGAL);
}

ns_value operator- (const ns_value &l, const ns_value &r) {
    if (l.type == r.type && l.type == NSVAL_INTEGER) {
        return ns_value(l.int_val - r.int_val);
    }
    else return ns_value(NSVAL_ILLEGAL);
}

ns_value operator* (const ns_value &l, const ns_value &r) {
    if (l.type == r.type && l.type == NSVAL_INTEGER) {
        return ns_value(l.int_val * r.int_val);
    }
    else return ns_value(NSVAL_ILLEGAL);  
}

ns_value operator/ (const ns_value &l, const ns_value &r) {
    if (l.type == r.type && l.type == NSVAL_INTEGER && r.int_val != 0) {
        return ns_value(l.int_val / r.int_val);
    }
    else return ns_value(NSVAL_ILLEGAL);  
}

ns_value operator% (const ns_value &l, const ns_value &r) {
    if (l.type == r.type && l.type == NSVAL_INTEGER && r.int_val != 0) {
        return ns_value(l.int_val % r.int_val);
    }
    else return ns_value(NSVAL_ILLEGAL);  
}

bool operator == (const ns_value &l, const ns_value &r) {
    bool v = false;
    if (l.type != r.type) {
        std::cerr 
            << "warning! can't compare the values with different type." 
            << std::endl;
        return v;
    }
    switch (l.type) {
        case NSVAL_STATUS: /* the same with integer */
        case NSVAL_INTEGER:
            v = (l.int_val == r.int_val) ? true : false;
            break;
        case NSVAL_LITERAL_STR:
            v = (*l.chr_val == *r.chr_val) ? true : false;
            break;
        case NSVAL_BOOLEAN:
            v = (l.bool_val == r.bool_val) ? true : false;
            break;
        default:
            std::cerr 
                << "warning. can't convert to boolean from type:" 
                << l 
                << std::endl;
            break; 
    }
    return v;
}

bool operator != (const ns_value &l, const ns_value &r) {
    return ! operator== (l, r);
}

bool operator > (const ns_value &l, const ns_value &r) {
    bool v = false;
    if (l.type != r.type) {
        std::cerr 
            << "warning! can't compare the values with different type." 
            << std::endl;
        return v;
    }
    switch (l.type) {
        case NSVAL_STATUS: /* the same with integer */
        case NSVAL_INTEGER:
            v = (l.int_val > r.int_val) ? true : false;
            break;
        case NSVAL_LITERAL_STR:
            v = (*l.chr_val > *r.chr_val) ? true : false;
            break;
        case NSVAL_BOOLEAN:
            v = (l.bool_val > r.bool_val) ? true : false;
            break;
        default:
            std::cerr
                << "warning. can't convert to boolean from type:"
                << l << std::endl;
            break; 
    }
    return v;
}

bool operator <  (const ns_value &l, const ns_value &r) {
    return !(operator> (l, r));
}

bool operator <= (const ns_value &l, const ns_value &r) {
    return operator== (l, r) || operator< (l, r);
}

bool operator >= (const ns_value &l, const ns_value &r) {
    return operator== (l, r) || operator> (l, r);
}


ns_value get_elem(const ns_value &n, unsigned int index) {
    if (n.type ==  NSVAL_LIST) {
        std::list<ns_value>::iterator it = n.list_val->begin();
        int i = 0;
        for (i = 0; i<index && it != n.list_val->end(); ++it, ++i) {
        }
        if (i == index && it != n.list_val->end())
            return *it;
    }

    return ns_value(NSVAL_ILLEGAL);
}

const ns_value& set_elem(const ns_value &n, unsigned int index, const ns_value& v) {
    
    if (n.type ==  NSVAL_LIST) {
        std::list<ns_value>::iterator it = n.list_val->begin();
        int i = 0;
        for (i = 0; i<index && it != n.list_val->end(); ++it, ++i) {
        }
        if (i == index && it != n.list_val->end())
            *it = v;
    }
    return n;
}
