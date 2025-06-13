#ifndef STRING_H
#define STRING_H
/*
MIT License

Copyright (c) 2025 Tristan Styles

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and atsociated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//------------------------------------------------------------------------------

#include <hol/holib.h>  // https://github.com/stytri/hol
#include "map.h"

//------------------------------------------------------------------------------

typedef struct {
	size_t      len;
	void const *str;
}
	STRING;
#define STRING(STRING__len,STRING__str) \
	(STRING){ .len = (STRING__len), .str = (STRING__str) }

static int case_equal(STRING l, STRING r) {
	return (l.len == r.len) && (memcmp(l.str, r.str, r.len) == 0);
}

static inline int memcasecmp(void const *csp, void const *ctp, size_t n) {
	char const *cs  = csp;
	char const *ct  = ctp;
	for(char const *end = cs + n; cs < end; cs++, ct++) {
		if(toupper(*cs) != toupper(*ct)) {
			return toupper(*cs) - toupper(*ct);
		}
	}
	return 0;
}

static int nocase_equal(STRING l, STRING r) {
	return (l.len == r.len) && (memcasecmp(l.str, r.str, r.len) == 0);
}

static int (*equal)(STRING l, STRING r) = case_equal;

typedef struct {
	uint64_t h;
	size_t   ref;
	STRING   str;
}
	STRTAB;

#define STRTAB(STRTAB__size)  calloc((STRTAB__size), sizeof(STRTAB));

static inline int string_eq(uint64_t h, void const *t, void const *p) {
	return (((STRTAB const *)t)->h == h)
		&& equal(((STRTAB const *)t)->str, *(STRING const *)p)
	;
}

static inline int string_q(void const *t) {
	return (((STRTAB *)t)->ref == 0);
}

static inline STRTAB *string_entry(size_t n_strings, STRTAB *tab, STRING str, uint64_t h) {
	return map(
		n_strings,
		sizeof(*tab),
		tab,
		&str,
		h,
		string_eq,
		string_q
	);
}

static STRING *string_lookup(size_t n_strings, STRTAB *tab, STRING str) {
	uint64_t h = hash(str.str, str.len);
	STRTAB  *s = string_entry(n_strings, tab, str, h);
	return (s && (s->ref != 0)) ? &s->str : NULL;
}

static STRING *string_intern(size_t n_strings, STRTAB *tab, STRING str, int *isnewp) {
	uint64_t h = hash(str.str, str.len);
	STRTAB  *s = string_entry(n_strings, tab, str, h);
	if(s) {
		if((*isnewp = (s->ref == 0))) {
			s->h   = h;
			s->str = str;
		}
		s->ref += !!~s->ref;
		return &s->str;
	}
	return NULL;
}

//------------------------------------------------------------------------------

#endif//ndef STRING_H
