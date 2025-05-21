#ifndef MAP_H
#define MAP_H
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

static uint64_t hash(void const *cp, size_t n) {
	uint64_t       h = 0;
	uint64_t const e = UINT64_C(2718281828459045235);  // e * 10**18
	for(uint8_t const *cb = cp; n > 0; n--) {
		h = ((h + n) * e) ^ (*cb++ * e);
	}
	return h;
}

static inline void *map(
	size_t   n,
	size_t   z,
	void    *tab,
	void    *p,
	uint64_t h,
	int    (*eq)(uint64_t h, void const *t, void const *p),
	int    (*q)(void const *t)
) {
	unsigned const k = (((h / n) << 1) % n) | 1;
	uint64_t const y = h % n;
	uint64_t       x = y;
	do {
		unsigned i = x;
		void    *t = &((char *)tab)[i * z];
		if(eq(h, t, p) || q(t)) {
			return t;
		}
		x = (x + k) % n;
	} while(x != y)
		;
	return NULL;
}

//------------------------------------------------------------------------------

#endif//ndef MAP_H
