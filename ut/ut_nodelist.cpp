
#include "ns_ast.h"

typedef node_list<node, node::EXP_NODE>  stmt_list;

class mynode : public explist_base {
public:
    mynode(): explist_base() {}

    ns_value eval() {
        nl_iter it = begin();
        for (; it != end(); ++it) {
            std::cout << (*it)->eval() << std::endl;
        }
        return ns_value(NSVAL_STATUS,NSVAL_STATUS_OK);
    }
};

void test() {
    stmt_list  sl;
    char s[] = "string";
    sl.append(new int_node(9999));
    sl.append(new str_node(s));
    sl.append(new int_node(888));
    
    stmt_list::nl_iter it = sl.begin();
    for (; it != sl.end(); ++it) {
        std::cout << (*it)->eval() << std::endl;
    }

    mynode s2;
    s2.append(new int_node(9999));
    s2.append(new int_node(888));
    s2.eval();
}

int main() {
    test();
	return 0;
}
