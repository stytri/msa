#ifndef SYMFILE_H
#define SYMFILE_H
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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

//------------------------------------------------------------------------------

#define SYMFILE_VERSION  0

//------------------------------------------------------------------------------

typedef struct {
	uint32_t name;
	uint32_t info;
	uint64_t data;
}
	symfile_symbol;

//------------------------------------------------------------------------------

static int symfile__range_index(int c) {
	static char const chars[] =
		// ASCII order
		"$"
		"0123456789"
		"ABCDEFGHIJKLMNOPQRTSUVWXYZ"
		"_"
		"abcdefghijklmnopqrtsuvwxyz"
	;
	char const *cp = strchr(chars, c);
	return cp ? cp - chars : - 1;
}

static void symfile_append(
	uint32_t       ri[64],
	int           *rip,
	symfile_symbol st[],
	uint32_t      *sip,
	char           ns[],
	uint32_t      *nop,
	size_t         nlen,
	char const    *name,
	uint32_t       info,
	uint64_t       data
) {
	uint32_t i = (*sip)++;
	uint32_t noff = (*nop); *nop += nlen + 1;
	memcpy(ns + noff, name, nlen); ns[noff + nlen] = '\0';
	st[i].name = noff,
	st[i].info = info;
	st[i].data = data;
	for(int x = symfile__range_index(*name); *rip < x; ) {
		ri[++(*rip)] = i;
	}
}

#ifdef _WIN32
static int symfile_lookup__cmp(void *ctx, const void *key, const void *p) {
#else
static int symfile_lookup__cmp(const void *key, const void *p, void *ctx) {
#endif
	return strcmp(key, (char const *)ctx + ((symfile_symbol const *)p)->name);
}

static symfile_symbol *symfile_lookup(
	uint32_t       ri[64],
	uint32_t       sz,
	symfile_symbol st[sz],
	uint32_t       nz,
	char           ns[nz],
	char const    *name
) {
	int      x  = symfile__range_index(*name);
	uint32_t lo = (x >  0) ? ri[x    ] : 0;
	uint32_t hi = (x < 63) ? ri[x + 1] : sz;
	size_t   n  = hi - lo;
	if(n > 0) {
		return bsearch_s(name, &st[lo], n, sizeof(*st), symfile_lookup__cmp, ns);
	}
	return NULL;
}

static int symfile_write(
	uint8_t        hd[16],
	uint32_t       ri[64],
	uint32_t       sz,
	symfile_symbol st[sz],
	uint32_t       nz,
	char           ns[nz],
	uint32_t       dz,
	void          *ds,
	FILE          *fp
) {
	int err = 0;
	if((err == 0) && (fwrite("SYM", 1         ,  4, fp) !=  4)) err = errno;
	if((err == 0) && (fwrite(hd , sizeof(*hd ), 16, fp) != 12)) err = errno;
	if((err == 0) && (fwrite(&sz, sizeof( sz ),  1, fp) !=  1)) err = errno;
	if((err == 0) && (fwrite(&nz, sizeof( nz ),  1, fp) !=  1)) err = errno;
	if((err == 0) && (fwrite(&dz, sizeof( dz ),  1, fp) !=  1)) err = errno;
	if((err == 0) && (fwrite(ri , sizeof(*ri ), 64, fp) != 64)) err = errno;
	if((err == 0) && (fwrite(st , sizeof(*st ), sz, fp) != sz)) err = errno;
	if((err == 0) && (fwrite(ns , sizeof(*ns ), nz, fp) != nz)) err = errno;
	if((err == 0) && (fwrite(ds , 1           , dz, fp) != dz)) err = errno;
	return err;
}

static int symfile_read(
	uint8_t          hd[16],
	uint32_t         ri[64],
	uint32_t        *szp,
	symfile_symbol **stp,
	uint32_t        *nzp,
	char           **nsp,
	uint32_t        *dzp,
	void           **dsp,
	FILE            *fp
) {
	int err = 0;
	uint8_t         mgc[4];
	uint32_t        sz = 0;
	uint32_t        nz = 0;
	uint32_t        dz = 0;
	symfile_symbol *st = NULL;
	char           *ns = NULL;
	void           *ds = NULL;
	if((err == 0) && (fread(mgc, sizeof(*mgc),  4, fp) !=  4)) err = errno;
	if((err == 0) && (memcmp(mgc, "SYM"      ,  4)     !=  0)) err = EILSEQ;
	if((err == 0) && (fread(hd , sizeof(*hd ), 16, fp) != 12)) err = errno;
	if((err == 0) && (fread(&sz, sizeof( sz ),  1, fp) !=  1)) err = errno;
	if((err == 0) && (fread(&nz, sizeof( nz ),  1, fp) !=  1)) err = errno;
	if((err == 0) && (fread(&dz, sizeof( dz ),  1, fp) !=  1)) err = errno;
	if((err == 0) && (fread(ri , sizeof(*ri ), 64, fp) != 64)) err = errno;
	if((err == 0) && ((st = calloc(sz, sizeof(*st))) != NULL)) err = errno;
	if((err == 0) && ((ns = calloc(nz, sizeof(*ns))) != NULL)) err = errno;
	if((err == 0) && ((ds = calloc(dz, 1          )) != NULL)) err = errno;
	if((err == 0) && (fread(st , sizeof( *st), sz, fp) != sz)) err = errno;
	if((err == 0) && (fread(ns , sizeof( *ns), nz, fp) != nz)) err = errno;
	if((err == 0) && (fread(ds , 1           , dz, fp) != dz)) err = errno;
	*szp = sz; *stp = st;
	*nzp = nz; *nsp = ns;
	*dzp = dz; *dsp = ds;
	return err;
}

//------------------------------------------------------------------------------

#endif//ndef SYMFILE_H
