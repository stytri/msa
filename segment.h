#ifndef SEGMENT_H
#define SEGMENT_H
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

#include <segfile.h>

//------------------------------------------------------------------------------

static inline void *segment__pointer(SEGMENT *sp, size_t size, size_t addr) {
	size_t off = addr - sp->start;
	if(sp->len < (off + 1)) {
		sp->len = (off + 1);
	}
	return (char *)sp->base + (off * size);
}

static void *segment__merge(SEGLIST *sl, SEGMENT *sp, size_t size, size_t addr) {
	size_t z = sp->next->end - sp->start;
	void  *p = realloc(sp->base, z * size);
	if(!p) {
		return NULL;
	}
	sp->base = p;
	sp->end = sp->start + z;
	z = sp->next->end - sp->next->start;
	p = segment__pointer(sp, size, sp->next->start);
	memcpy(p, sp->next->base, z * size);
	p = sp->next;
	sp->next = ((SEGMENT *)p)->next;
	free(((SEGMENT *)p)->base);
	free(p);
	if(sl->tail == p) {
		sl->tail = sp;
	}
	return segment__pointer((sl->current = sp), size, addr);
}

static void *segment__expand(SEGLIST *sl, SEGMENT *sp, size_t size, size_t addr) {
	size_t z = (((addr - sp->start) / sl->granularity) + 1) * sl->granularity;
	size_t n = ((sp->end - sp->start) / sl->granularity) * sl->granularity;
	void  *p = realloc(sp->base, z * size);
	if(!p) {
		return NULL;
	}
	memset((char *)p + (n * size), 0, (z - n)  * size);
	sp->base = p;
	sp->end = sp->start + z;
	if(sp->next && (sp->end == sp->next->start)) {
		if(sl->tail == sp->next) {
			sl->tail = sp;
		}
		return segment__merge(sl, sp, size, addr);
	}
	return segment__pointer((sl->current = sp), size, addr);
}

static void *segment__create(SEGLIST *sl, size_t size, size_t addr) {
	SEGMENT *sp = malloc(sizeof(*sp));
	if(!sp) {
		return NULL;
	}
	sp->base = calloc(size, sl->granularity);
	if(!sp->base) {
		return NULL;
	}
	sp->start = addr - (addr % sl->granularity);
	sp->end   = sp->start + sl->granularity;
	sp->len   = 0;
	sp->next  = NULL;
	if(sl->head) {
		if(sl->tail->end == sp->start) {
			sl->tail->next = sp;
			return segment__merge(sl, sl->tail, size, addr);
		}
		if(sl->tail->end < sp->start) {
			sl->tail->next = sp;
			sl->tail = sp;
			return segment__pointer((sl->current = sp), size, addr);
		}
		if(sp->end < sl->head->start) {
			sp->next = sl->head;
			sl->head = sp;
			return segment__pointer((sl->current = sp), size, addr);
		}
		if(sp->end == sl->head->start) {
			sp->next = sl->head;
			sl->head = sp;
			return segment__merge(sl, sp, size, addr);
		}
		for(SEGMENT *curr = sl->head, *end = sl->tail; curr != end; curr = curr->next) {
			if((addr >= curr->end) && (addr < curr->next->start)) {
				sp->next = curr->next;
				curr->next = sp;
				if(curr->end == sp->start) {
					segment__merge(sl, curr, size, addr);
					sp = sl->current;
				}
				if(sp->end == sp->next->start) {
					return segment__merge(sl, sp, size, addr);
				}
				return segment__pointer((sl->current = sp), size, addr);
			}
		}
		return NULL;
	}
	sl->head = sl->tail = sp;
	return segment__pointer((sl->current = sp), size, addr);
}

static inline void *segment_pointer(SEGLIST *sl, size_t size, size_t addr) {
	SEGMENT *sp = sl->current;
	if(sp) {
		SEGMENT *next;
		if(addr < sp->start) {
			SEGMENT *end = sp;
			for(sp = sl->head; (sp != end) && (addr >= sp->start); sp = next) {
				if(addr < sp->end) {
					return segment__pointer(sp, size, addr);
				}
				next = sp->next;
				if(addr == sp->end) {
					if(next && (addr == next->start)) {
						return segment__merge(sl, sp, size, addr);
					}
					return segment__expand(sl, sp, size, addr);
				}
			}
		} else do {
			if(addr < sp->end) {
				return segment__pointer(sp, size, addr);
			}
			next = sp->next;
			if(addr == sp->end) {
				if(next && (addr == next->start)) {
					return segment__merge(sl, sp, size, addr);
				}
				return segment__expand(sl, sp, size, addr);
			}
		} while((sp = next) && (addr >= sp->start))
			;
	}
	return segment__create(sl, size, addr);
};

//------------------------------------------------------------------------------

#endif//ndef SEGMENT_H
