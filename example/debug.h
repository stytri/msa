#ifndef DEBUG__H
#define DEBUG__H
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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//------------------------------------------------------------------------------

enum { N_BREAKPOINTS = 16 };

struct debug {
	bool          step;
	int           n_breakpoints;
	unsigned long breakpoint[N_BREAKPOINTS];
	char          buf[BUFSIZ];
};

//------------------------------------------------------------------------------

typedef void (*funct_disasm)(unsigned long, struct debug *);
typedef int  (*funct_debug) (unsigned long, struct debug *, funct_disasm);

static int nodebug(
	unsigned long,
	struct debug *,
	funct_disasm
) {
	return 1;
}

static size_t debug_iscmd(char const *cs, char const *ct) {
	size_t n = strlen(ct);
	n = (strncmp(cs, ct, n) || isalpha(cs[n])) ? 0 : n;
	return n;
}

static inline size_t debug_gets(size_t bufsiz, char buf[bufsiz]) {
	size_t n = 0;
	int    c = 0;
	while(n < (bufsiz-1)) {
		c = getchar();
		if((c == '\n') || (c == EOF)) {
			return 0;
		}
		if(isgraph(c)) {
			buf[n++] = c;
			break;
		}
	}
	while(n < (bufsiz-1)) {
		c = getchar();
		if((c == '\n') || (c == EOF)) {
			break;
		}
		buf[n++] = c;
	}
	while((c != '\n') && (c != EOF)) {
		c = getchar();
	}
	while((n > 0) && !isgraph(buf[n-1])) {
		n--;
	}
	buf[n] = '\0';
	return n;
}

static int debug(
	unsigned long   pc,
	struct debug   *state,
	funct_disasm disasm
) {
	for(int i = 0; !state->step && (i < state->n_breakpoints); i++) {
		state->step = (pc == state->breakpoint[i]);
	}
	if(state->step) {
		disasm(pc, state);
		for(bool repeat = true; repeat; ) {
			repeat = false;
			putchar('>');
			putchar(' ');
			fflush(stdout);
			size_t n = debug_gets(sizeof(state->buf), state->buf);
			if(n > 0) {
				char const *buf = state->buf;
#				define COMMAND(COMMAND__cmd,COMMAND__short,COMMAND__long) \
				if((n = debug_iscmd(COMMAND__cmd, (COMMAND__short))) \
				|| (n = debug_iscmd(COMMAND__cmd, (COMMAND__long))) \
				)
				COMMAND(buf, "s", "step") {
					continue;
				}
				COMMAND(buf, "r", "run") {
					state->step = false;
					continue;
				}
				repeat = true;
				COMMAND(buf, "b", "break") {
					unsigned long bp = strtoul(buf + n, NULL, 0);
					if(!bp) {
						bp = pc;
					}
					if(state->n_breakpoints < N_BREAKPOINTS) {
						state->breakpoint[state->n_breakpoints++] = bp;
						continue;
					}
					printf("too many breakpoints\n");
					continue;
				}
				COMMAND(buf, "cla", "clearall") {
					state->n_breakpoints = 0;
					continue;
				}
				COMMAND(buf, "cl", "clear") {
					unsigned long bp = strtoul(buf + n, NULL, 0);
					if(!bp) {
						bp = pc;
					}
					for(int i = 0; i < state->n_breakpoints; i++) {
						if(pc == state->breakpoint[i]) {
							for(state->n_breakpoints--; i < state->n_breakpoints; i++) {
								state->breakpoint[i] = state->breakpoint[i+1];
							}
							break;
						}
					}
					continue;
				}
				COMMAND(buf, "q", "quit") {
					return 0;
				}
				printf("unknown debug command: %s\n", buf);
#				undef COMMAND
			}
		}
	}
	return 1;
}

//------------------------------------------------------------------------------

#endif//ndef DEBUG__H
