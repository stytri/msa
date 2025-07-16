#ifndef SYMBOL_H
#define SYMBOL_H
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
#include "string.h"
#include "value.h"
#include "map.h"

//------------------------------------------------------------------------------

typedef struct {
	int      type;
	uint64_t tag;
	STRING   sym;
	STRING   rpl;
	VALUE    val;
}
	SYMBOL;

#define HIDDEN_SYMBOL  (UINT64_C(1) << 31)
#define DELIMETER_SYMBOL  (UINT64_C(1) << 30)

typedef struct {
	uint64_t h;
	size_t   ref;
	SYMBOL   sym;
}
	SYMTAB;

static inline int symbol_eq(uint64_t h, void const *t, void const *p) {
	return (((SYMTAB const *)t)->h == h)
		&& equal(((SYMTAB const *)t)->sym.sym, *(STRING const *)p)
	;
}

static inline int symbol_q(void const *t) {
	return (((SYMTAB *)t)->ref == 0);
}

static inline SYMTAB *symbol_entry(size_t n_symbols, SYMTAB *tab, STRING sym, uint64_t h) {
	return map(
		n_symbols,
		sizeof(*tab),
		tab,
		&sym,
		h,
		symbol_eq,
		symbol_q
	);
}

static SYMBOL *symbol_lookup(size_t n_symbols, SYMTAB *tab, STRING sym) {
	uint64_t h = hash(sym.str, sym.len);
	SYMTAB  *s = symbol_entry(n_symbols, tab, sym, h);
	return (s && (s->ref != 0)) ? &s->sym : NULL;
}

static SYMBOL *symbol_intern(size_t n_symbols, SYMTAB *tab, int type, uint64_t tag, STRING sym, STRING rpl, VALUE val) {
	uint64_t h = hash(sym.str, sym.len);
	SYMTAB  *s = symbol_entry(n_symbols, tab, sym, h);
	if(s) {
		if(s->ref == 0) {
			s->h        = h;
			s->sym.type = type;
			s->sym.tag  = tag;
			s->sym.sym  = sym;
			s->sym.rpl  = rpl;
			s->sym.val  = val;
		}
		s->ref += !!~s->ref;
		return &s->sym;
	}
	return NULL;
}

//------------------------------------------------------------------------------

#endif//ndef SYMBOL_H
