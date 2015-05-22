
#include <cstdlib>
#include <cstring>
#include "ns_util.h"

char* make_raw_str_buffer(size_t n) {
    char* buff = (char *)malloc(n);
    memset(buff, 0x00, n);
    return buff;
}

void  free_raw_str(char *s) {
    if (s != NULL) free(s);
}
