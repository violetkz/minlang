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

#ifndef ns_util__hpp
#define ns_util__hpp
#include "ns_value.h"

/* raw string buffer */
char* make_raw_str_buffer(size_t n);
/* free the string buffer */
void  free_raw_str(char*);

/* handle eval status */
const ns_value eval_status_ok(NSVAL_STATUS, NSVAL_STATUS_OK);
const ns_value eval_illegal(NSVAL_ILLEGAL);

/* runtime panic */
std::ostream &ns_panic(int lineno);
std::ostream &ns_trace(int lineno);
#endif
