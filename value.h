#ifndef VALUE__H
#define VALUE__H
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

//------------------------------------------------------------------------------

typedef struct {
	unsigned long      type;
	union {
	unsigned long long u;
	double             f;
	void     const    *p;
	};
}
	VALUE;

#define FIRST(FIRST__1st,...)          (FIRST__1st)
#define REMAINING(REMAINING__1st,...)  __VA_ARGS__
#define SECOND(...)                    FIRST(REMAINING(__VA_ARGS__)+0)

#define VALUE(VALUE__type,...) \
	(VALUE){ \
		.type = (SECOND(__VA_ARGS__) << 8) | *#VALUE__type, \
		.VALUE__type = FIRST(__VA_ARGS__) \
	}


static VALUE strtoval(char const *cs, char **endp) {
	VALUE v = VALUE(u, 0);
	char *s;
	if(!endp) endp = &s;
	for(; isblank(*cs); cs++)
		;
	if(*cs == '\'') {
		for(int c = *++cs; c && (c != '\''); c = *++cs) {
			if(c == '\\') {
				c = *++cs;
				if(!c) break;
				switch(c) {
				case 'a': c = '\a'; break;
				case 'b': c = '\b'; break;
				case 'f': c = '\f'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				case 'v': c = '\v'; break;
				}
			} else if(c == '^') {
				c = *++cs;
				if(!c) break;
				c &= 0x1fu;
			}
			v.u = (v.u << 8) | (c & 0xffu);
		}
		if(*cs == '\'') cs++;
		*endp = (char *)cs;
	} else {
		v = VALUE(u, strtoull(cs, endp, 0));
		switch(**endp) {
		case '.':
		case 'E': case 'e':
		case 'P': case 'p':
			v = VALUE(f, strtod(cs, endp));
			break;
		}
	}
	return v;
}

//------------------------------------------------------------------------------

#endif//ndef VALUE__H
