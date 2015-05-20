
#include "ns_value.h"
#include <iostream>

void constructor_test () {
    ns_value n(99);     std::cout << n;
    ns_value nb(false);  std::cout << nb;
    ns_value ns("Test~");  std::cout << ns;
    
    ns_value xn(NSVAL_ILLEGAL); std::cout << xn;
    ns_value xn2(NSVAL_UNINITIALIZED); std::cout << xn2; 
    ns_value n1 = -222; std::cout << n1;
    ns_value n2 = true; std::cout << n2;
    ns_value n3 = "wwewtpetept"; std::cout << n3;
    ns_value n4 = NSVAL_UNINITIALIZED; std::cout << n4;
}

void assign_test() {
    ns_value nss("assign_test-----");
    ns_value nss_1 = "initialized string";
    ns_value nss_2 = nss;
    ns_value nss_3 = nss;
    ns_value nss_4 = nss_2;
    nss_2 = nss;

    nss_4 = nss_1 = nss_2 = nss_4;
    std::cout << nss_4;
}

void assign_test1() {
    ns_value nss("assign_test-----");
    ns_value nint = 9999;
    
    nss = nint;
    std::cout << nss;
    
    nss = "string";
    nint = true;
    
    nss  = nint;
    std::cout << nss;

    ns_value n1,n2,n3,n4;
    n1 = false;
    n2 = "string";
    n4 = NSVAL_ILLEGAL;
    
    n4 = n2 = n1;
    std::cout << n4;
}

void opt_test() {
    ns_value ns = 9;
    ns_value ns1 = 10000;
    
    std::cout << ns * ns1;
    
    ns = "99999";
    ns1 = "--0000";
    ns_value ns3 = "!!";
    std::cout << ns + ns1 + ns3 + ns1;

    ns = true;
    ns1 = 9;
    std::cout << ns + ns1;
}

void opt_test2() {
    ns_value n= "hello";
    ns_value n1 = "world";
    
    std::cout << n + n1 ;
}
int main() {
//    constructor_test();
    assign_test();
    assign_test1();
    opt_test();
}
