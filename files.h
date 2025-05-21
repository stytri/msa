#ifndef FILES_H
#define FILES_H
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

char *
loadsourcefiles(
	int   filc,
	char *filv[]
) {
	static char const line[] = "\n#line 1 \"%s\"\n";
	char  *s = NULL;
	size_t z = 0;
	size_t n = 0;
	int    i = 0;
	do {
		char *file = "";
		FILE *f    = stdin;
		if(i < filc) {
			file = filv[i++];
			f    = fopen(file, "r");
		}
		if(!f) {
			if(s) s = (free(s), NULL);
			return s;
		}
		size_t m = BUFSIZ, w;
		if(f != stdin) {
			if(fseek(f, 0, SEEK_END) == 0) {
				long l = ftell(f);
				if(l > 0) {
					m = (size_t)l;
				}
			}
			rewind(f);
		}
		w = sizeof(line) + strlen(file);
		w = roundup(BUFSIZ, n + w + m + 1) - 1;
		if(w != z) {
			void *p = realloc(s, w + 1);
			if(!p) {
				if(s) s = (free(s), NULL);
				return s;
			}
			s = p;
			z = w;
		}
		w = sprintf(&s[n], line, file);
		n += w;
		while(!feof(f)) {
			w = roundup(BUFSIZ, n + m + 1) - 1;
			if(w != z) {
				void *p = realloc(s, w + 1);
				if(!p) {
					if(s) s = (free(s), NULL);
					return s;
				}
				s = p;
				z = w;
			}
			m = z - n;
			w = fread(s + n, sizeof(char), m, f);
			if(ferror(f)) {
				if(s) s = (free(s), NULL);
				if(f != stdin) fclose(f);
				return s;
			}
			n += w;
			m = BUFSIZ;
		}
		s[n] = '\0';
		if(f != stdin) fclose(f);
		f = NULL;
	} while(i < filc)
		;
	return s;
}

//------------------------------------------------------------------------------

#endif//ndef FILES_H
