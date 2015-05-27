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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "ns_util.h"

char* make_raw_str_buffer(size_t n) {
    char* buff = (char *)malloc(n);
    memset(buff, 0x00, n);
    return buff;
}

void  free_raw_str(char *s) {
    if (s != NULL) free(s);
}
 
std::ostream &ns_panic(int lineno) {
    return std::cerr<< "-E- line[" << lineno << "] ";
}

std::ostream &ns_trace(int lineno) {
    return std::cerr<< "-T- line[" << lineno << "] ";
}
