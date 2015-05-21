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


#ifndef ns_value_h___
#define ns_value_h___

#include <iostream>
#include <vector>


enum ns_value_type {
    NSVAL_INTEGER       = 0, /* integer */
    NSVAL_LITERAL_STR   = 1, /* literal string */
    NSVAL_BOOLEAN       = 2, /* boolean */
    NSVAL_EXPERESS_AST  = 3, /* expression  */
    NSVAL_UNINITIALIZED = 4, /* uninitialized */
    NSVAL_ILLEGAL       = 5, /* illegal */
    NSVAL_STATUS        = 6, /* stmt stauts */
    NSVAL_LIST          = 7, /* list */
    NSVAL_CHAR          = 8, /* char */
};

enum ns_status_type {
    NSVAL_STATUS_OK, 
    NSVAL_STATUS_FAILED,
    NSVAL_STATUS_RETURN,  
    NSVAL_STATUS_BREAK,
    NSVAL_STATUS_CONTINUE
};

struct node; //previous declaration

class ns_value {
public:
    ns_value_type type;
    union {
        //char                    chr_val;    /* value for char */
        int                     int_val;    /* value for integer or status */
        bool                    bool_val;   /* value for boolean */
        std::string             *chr_val;   /* value for string  */
        node                    *node_val;  /* runnable expression */
        std::vector<ns_value>   *list_val;  /* value for array    */
    };

    typedef std::vector<ns_value>::iterator array_iter;
public: 
    ns_value():type(NSVAL_UNINITIALIZED), int_val(0), ref_count(0) {}
    ns_value(const ns_value& n);
    ~ns_value();

    explicit ns_value(int v) : type(NSVAL_INTEGER), int_val(v) ,ref_count(0) {}
    explicit ns_value(bool b) : type(NSVAL_BOOLEAN), bool_val(b) ,ref_count(0){}
    explicit ns_value(const char *s);
    explicit ns_value(ns_value_type t);
    explicit ns_value(ns_value_type t, ns_status_type status) 
            : type(t), int_val(status), ref_count(0){}
    inline int count() const {return (ref_count) ? *ref_count : 0;}
    ns_value &operator = (const ns_value &s);

    operator bool();
    
    explicit ns_value(node *exp):type(NSVAL_EXPERESS_AST), node_val(exp) {}

    inline bool is_int() const {
        return (type == NSVAL_INTEGER); 
    }

    inline bool is_illegal_value() const { 
        return (type == NSVAL_ILLEGAL);
    }
    
    inline bool is_array() const {
        return type == NSVAL_LIST;
    }

    inline bool is_iteratale() const { 
        return (type == NSVAL_LITERAL_STR || type == NSVAL_LIST); 
    }

    inline bool is_status_return() const {
        return type == NSVAL_STATUS && int_val == NSVAL_STATUS_RETURN;
    }

    inline bool is_status_break() const {
        return type == NSVAL_STATUS && int_val == NSVAL_STATUS_BREAK;
    }

    inline bool is_status_continue() const {
        return type == NSVAL_STATUS && int_val == NSVAL_STATUS_CONTINUE;
    }

    inline bool is_status_ok() const {
        return  (type == NSVAL_STATUS && int_val == NSVAL_STATUS_OK) 
                || (type != NSVAL_ILLEGAL) 
                || (type != NSVAL_UNINITIALIZED);
    }

    /* some builtin func for array type */
    ns_value get_elem(unsigned int index);
    void     set_elem(unsigned int index, const ns_value &v);
    void    append(const ns_value &v);
    int  len();
    void    del(size_t idx);
    int    find(const ns_value &v);

private:
    inline void add_ref(); 
    inline void release();
    inline void destruct();
    static bool is_ref_count_type(ns_value_type t);
private:
    static const int need_ref_count_bit_map;
    int *ref_count;
};

std::ostream &operator << (std::ostream &out, const ns_value &v);
ns_value operator+ (const ns_value &l, const ns_value &r);
ns_value operator- (const ns_value &l, const ns_value &r);
ns_value operator* (const ns_value &l, const ns_value &r);
ns_value operator/ (const ns_value &l, const ns_value &r);
ns_value operator% (const ns_value &l, const ns_value &r);

bool operator == (const ns_value &l, const ns_value &r);
bool operator != (const ns_value &l, const ns_value &r);
bool operator >  (const ns_value &l, const ns_value &r);
bool operator <  (const ns_value &l, const ns_value &r);
bool operator <= (const ns_value &l, const ns_value &r);
bool operator >= (const ns_value &l, const ns_value &r);


#endif  //~ns_value_h___
