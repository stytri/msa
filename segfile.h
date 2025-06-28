#ifndef SEGFILE_H
#define SEGFILE_H
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

#include <stdlib.h>
#include <string.h>
#include <error.h>

//------------------------------------------------------------------------------

typedef struct segment {
	size_t          start;
	size_t          end;
	size_t          len;
	void           *base;
	struct segment *next;
}
	SEGMENT;

typedef struct {
	size_t   granularity;
	SEGMENT *current;
	SEGMENT *head;
	SEGMENT *tail;
}
	SEGLIST;

//------------------------------------------------------------------------------

static int segfile_write(SEGLIST *sl, size_t sizeof_byte, size_t sizeof_memory, FILE *fp) {
	int err = 0;
	do {
		if(fwrite("SEG", 1, 4, fp) != 4) {
			err = errno;
			break;
		}
		if((sizeof_memory - 1) > UINT32_MAX) {
			uint32_t x = (64ul << 16) | sizeof(uint64_t);
			if(fwrite(&x, sizeof(x), 1, fp) != 1) {
				err = errno;
				break;
			}
			for(SEGMENT *sp = sl->head; sp; sp = sp->next) {
				size_t n = sp->len;
				uint64_t seg[2] = { sp->start, n };
				if(fwrite(seg, sizeof(*seg), 2, fp) != 2) {
					err = errno;
					break;
				}
				size_t z = fwrite(sp->base, sizeof_byte, n, fp);
				if(z != n) {
					err = errno;
					break;
				}
			}
			break;
		}
		if((sizeof_memory - 1) > UINT16_MAX) {
			uint32_t x = (32ul << 16) | sizeof(uint32_t);
			if(fwrite(&x, sizeof(x), 1, fp) != 1) {
				err = errno;
				break;
			}
			for(SEGMENT *sp = sl->head; sp; sp = sp->next) {
				size_t n = sp->len;
				uint32_t seg[2] = { sp->start, n };
				if(fwrite(seg, sizeof(*seg), 2, fp) != 2) {
					err = errno;
					break;
				}
				size_t z = fwrite(sp->base, sizeof_byte, n, fp);
				if(z != n) {
					err = errno;
					break;
				}
			}
			break;
		}
		if((sizeof_memory - 1) > UINT8_MAX) {
			uint32_t x = (16ul << 16) | sizeof(uint16_t);
			if(fwrite(&x, sizeof(x), 1, fp) != 1) {
				err = errno;
				break;
			}
			for(SEGMENT *sp = sl->head; sp; sp = sp->next) {
				size_t n = sp->len;
				uint16_t seg[2] = { sp->start, n };
				if(fwrite(seg, sizeof(*seg), 2, fp) != 2) {
					err = errno;
					break;
				}
				size_t z = fwrite(sp->base, sizeof_byte, n, fp);
				if(z != n) {
					err = errno;
					break;
				}
			}
			break;
		}
		uint32_t x = (8ul << 16) | sizeof(uint8_t);
		if(fwrite(&x, sizeof(x), 1, fp) != 1) {
			err = errno;
			break;
		}
		for(SEGMENT *sp = sl->head; sp; sp = sp->next) {
			size_t n = sp->len;
			uint16_t seg[2] = { sp->start, n };
			if(fwrite(seg, sizeof(*seg), 2, fp) != 2) {
				err = errno;
				break;
			}
			size_t z = fwrite(sp->base, sizeof_byte, n, fp);
			if(z != n) {
				err = errno;
				break;
			}
		}
	} while(0)
		;
	return err;
}

static int segfile_read(void *memory, size_t sizeof_byte, size_t sizeof_memory, FILE *fp) {
	char seg[4];
	if(fread(seg, 1, 4, fp) != 4) {
		return errno;
	}
	if(memcmp(seg, "SEG", 4) == 0) {
		uint32_t seg;
		if(fread(&seg, sizeof(seg), 1, fp) != 1) {
			return errno;
		}
		if(seg == ((64ul << 16) | sizeof(uint64_t))) {
			for(uint64_t seg[2]; fread(seg, sizeof(*seg), 2, fp) == 2;) {
				void *p = (char *)memory + (sizeof_byte * seg[0]);
				if(fread(p, sizeof_byte, seg[1], fp) != seg[1]) {
					return errno;
				}
			}
			return 0;
		}
		if(seg == ((32ul << 16) | sizeof(uint32_t))) {
			for(uint32_t seg[2]; fread(seg, sizeof(*seg), 2, fp) == 2;) {
				void *p = (char *)memory + (sizeof_byte * seg[0]);
				if(fread(p, sizeof_byte, seg[1], fp) != seg[1]) {
					return errno;
				}
			}
			return 0;
		}
		if(seg == ((16ul << 16) | sizeof(uint16_t))) {
			for(uint16_t seg[2]; fread(seg, sizeof(*seg), 2, fp) == 2;) {
				void *p = (char *)memory + (sizeof_byte * seg[0]);
				if(fread(p, sizeof_byte, seg[1], fp) != seg[1]) {
					return errno;
				}
			}
			return 0;
		}
		if(seg == ((8ul << 16) | sizeof(uint8_t))) {
			for(uint8_t seg[2]; fread(seg, sizeof(*seg), 2, fp) == 2;) {
				void *p = (char *)memory + (sizeof_byte * seg[0]);
				if(fread(p, sizeof_byte, seg[1], fp) != seg[1]) {
					return errno;
				}
			}
			return 0;
		}
		return errno = EILSEQ;
	}
	rewind(fp);
	fread(memory, sizeof_byte, sizeof_memory, fp);
	return ferror(fp) ? errno : 0;
}

//------------------------------------------------------------------------------

#endif//ndef SEGFILE_H
