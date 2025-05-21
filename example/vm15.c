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

//
// Build with https://github.com/stytri/m
//
// ::compile
// :+  $CC $CFLAGS $XFLAGS $SMALL-BINARY
// :+      -o $+^ $"* $"!
//
// ::clean
// :+  $RM *.i *.s *.o *.exe
//
// ::debug
// :+  $CC $CFLAGS
// :+      -Og -g -o $^-$+: $"!
// :&  $DBG -tui --args $^-$+: $"*
// :&  $RM $^-$+:
//
// ::-
// :+  $CC $CFLAGS $XFLAGS $SMALL-BINARY
// :+      -o $+: $"* $"!
//
// ::CFLAGS!CFLAGS
// :+      $CSTD -Wall -Wextra $WINFLAGS $INCLUDE
//
// ::XFLAGS!XFLAGS
// :+      -DNDEBUG=1 -O3 -march=native
//
// ::SMALL-BINARY
// :+      -fmerge-all-constants -ffunction-sections -fdata-sections
// :+      -fno-unwind-tables -fno-asynchronous-unwind-tables
// :+      -Wl,--gc-sections -s
//
// ::CSTD!CSTD
// :+      -std=c23
//
// ::windir?WINFLAGS
// :+      -D__USE_MINGW_ANSI_STDIO=1
//

//------------------------------------------------------------------------------

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "debug.h"

//------------------------------------------------------------------------------

static uint16_t mem[0100000u];
static uint16_t reg[010];
static uint16_t reg_previous[010];

#define PC reg[007]

static funct_debug debugger = nodebug;
static struct debug dbg_state = {
	.step = false,
	.n_breakpoints = 0,
};

//------------------------------------------------------------------------------

static uint16_t io_nul(uint16_t m, uint16_t *p) {
	(void)p;
	return m;
}

static uint16_t io_getchar(uint16_t m, uint16_t *p) {
	*p = (uint16_t)getchar();
	return m;
}

static uint16_t io_putchar(uint16_t m, uint16_t *p) {
	putchar((int)*p);
	return m;
}

static uint16_t (*io[010])(uint16_t, uint16_t *) = {
	io_nul,
	io_putchar,
	io_getchar,
	io_nul,
	io_nul,
	io_nul,
	io_nul,
	io_nul
};

static int      iox = 0;
static uint16_t iop;
static uint16_t iom;

static void disasm(unsigned long pc, struct debug *) {
	static const char *instruction[] = {
		"mov", "ior", "xor", "and", "rot", "add", "lda", "sta",
	};
	static const char *condition[] = {
		"nsk", "skp", "srz", "srn", "scz", "scn", "sez", "snz",
	};
	printf("\niop: ");
	for(uint16_t b = 0x80; b; b >>= 1) {
		if(iop & b) {
			putchar('1');
		} else {
			putchar('0');
		}
	}
	printf("\niom: ");
	for(uint16_t b = 0x80; b; b >>= 1) {
		if(iom & b) {
			putchar('1');
		} else {
			putchar('0');
		}
	}
	for(int i = 0; i < 8; i++) {
		if((i % 4) == 0) putchar('\n');
		printf("  [r%i] 0%.5"PRIo16, i, reg[i]);
		if(reg_previous[i] != reg[i]) {
			reg_previous[i] = reg[i];
			putchar('*');
			continue;
		}
		putchar(' ');
	}
	putchar('\n');
	uint16_t  o = mem[pc];
	int const i = (o       ) & 007;
	int const m = (o >> 003) & 007;
	int const c = (o >> 006) & 007;
	int const a = (o >> 011) & 007;
	int const q = (o >> 014) & 007;
	printf("0%.5"PRIo16": 0%.5"PRIo16, PC, o);
	if((o & 000777u) == 000776u) {
		o = mem[pc+1]; printf(" 0%.5"PRIo16, o);
	}
	printf(" ; %s r%u", instruction[i], a);
	switch(m) {
	case 0: printf(" 0%"PRIo16, c); break;
	case 1: printf(" 07777%"PRIo16, c); break;
	case 2: printf(" 0%"PRIo16, UINT16_C(1) << (c + 3)); break;
	case 3: printf(" 0%"PRIo16, (UINT16_C(1) << (c + 3)) - 1); break;
	case 4: printf(" r%"PRIo16, c); break;
	case 5: printf(" -r%"PRIo16, c); break;
	case 6: printf(" --r%"PRIo16, c); break;
	case 7: printf(" r%"PRIo16"++", c); break;
	}
	printf(" %s", condition[q]);
	putchar('\n');
}

static int monitor(void) {
	iox = (iox % 07) + 1;
	iop = mem[077770u];
	iom = 1u << iox;

	int r = debugger(PC, &dbg_state, disasm);
	if(r <= 0) {
		return r;
	}

	if(iop & (iom | 1u)) {
		if(iop & 1u) {
			return 0;
		}
		iop ^= io[iox](iom, &mem[077770u + iox]);
		mem[077770u] = iop;
	}
	return 1;
}

static inline uint16_t rotate(uint16_t x, unsigned n) {
	int r = n & 017;
	int l = 017 - r;
	if((n & 077760u) == 077760u) {
		x = ((x << l) & 0177777u) | ((x & 0177777u) >> r);
	} else {
		x = (x & 0100000u) | ((x << l) & 077777u) | ((x & 077777u) >> r);
	}
	return x;
}

static int run(void) {
	int v = 0;
	for(uint16_t o = mem[(PC = 0)]; (v = monitor()) > 0; ) {
		PC = (PC + 1) & 077777u;
		int const i = (o       ) & 007;
		int const m = (o >> 003) & 007;
		int const c = (o >> 006) & 007;
		int const a = (o >> 011) & 007;
		int const q = (o >> 014) & 007;
		uint16_t  cval;
		switch(m) {
		case 0: cval = c; break;
		case 1: cval = 077770u | c; break;
		case 2: cval = (1u << (c + 3)) & 077777u; break;
		case 3: cval = ((1u << (c + 4)) - 1) & 077777u; break;
		case 4: cval = reg[c] & 077777u; break;
		case 5: cval = -reg[c] & 077777u; break;
		case 6: cval = reg[c] = (reg[c] - 1) & 077777u;; break;
		case 7: cval = reg[c] & 077777u; reg[c] = (reg[c] + 1) & 077777u; break;
		}
		switch(i) {
		case 0: reg[a] = cval; break;
		case 1: reg[a] = reg[a] | cval; break;
		case 2: reg[a] = reg[a] ^ cval; break;
		case 3: reg[a] = reg[a] & (0100000u | cval); break;
		case 4: reg[a] = rotate(reg[a], cval); break;
		case 5: reg[a] = (reg[a] & 077777u) + cval; break;
		case 6: reg[a] = mem[cval] & 077777u; break;
		case 7: mem[cval] = reg[a] & 077777u; break;
		}
		switch(q) {
		case 0: cval = 0; break;
		case 1: cval = 1; break;
		case 2: cval =  !(reg[a] & 0077777u); break;
		case 3: cval = !!(reg[a] & 0077777u); break;
		case 4: cval =  !(reg[a] & 0100000u); break;
		case 5: cval = !!(reg[a] & 0100000u); break;
		case 6: cval =  !(reg[a] & 0077777u) ||  !(reg[a] & 0100000u); break;
		case 7: cval = !!(reg[a] & 0077777u) && !!(reg[a] & 0100000u); break;
		}
		o = mem[PC];
		if(cval) {
			PC = (PC + 1 + ((o & 000776u) == 000776u)) & 077777u;
			o = mem[PC];
		}
	}
	return v;
}

//------------------------------------------------------------------------------

#ifdef __MINGW64__
int _dowildcard = -1;
#endif

static void usage(FILE *out, char *arg0) {
	char *args = strrchr(arg0, '/');
#ifdef _WIN32
	if(!args) args = strrchr(arg0, '\\');
	if(!args) args = strrchr(arg0, ':');
#endif
	if(!args) args = arg0;
#ifdef _WIN32
	{
		char *ext = strrchr(args, '.');
		if(ext) *ext++ = '\0';
	}
#endif
	fprintf(out, "use: %s [OPTIONS] FILE\n", args);
	fprintf(out, "OPTIONS:\n");
	fprintf(out, "\t-h, --help    display help\n");
	fprintf(out, "\t-d, --debug   enable debug\n");
}

int main(int argc, char **argv) {
	int argi = 1;
#define OPTION(OPTION__args,OPTION__short,...)  \
		if((*(OPTION__short) && !strcmp((OPTION__args), ("-"OPTION__short))) \
			|| (*(__VA_ARGS__"") && !strcmp((OPTION__args), ("--"__VA_ARGS__""))))
	while(argi < argc) {
		char *args = argv[argi];
		OPTION(args, "h", "help") {
			argi++;
			usage(stdout, argv[0]);
			return EXIT_SUCCESS;
		}
		OPTION(args, "d", "debug") {
			argi++;
			debugger = debug;
			dbg_state.step = true;
			continue;
		}
		break;
	}
	if(argi >= argc) {
		usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	FILE *in = fopen(argv[argi], "rb");
	if(!in) {
		perror(argv[argi]);
		exit(EXIT_FAILURE);
	}
	fread(mem, sizeof(*mem), sizeof(mem)/sizeof(*mem), in);
	bool failed = ferror(in);
	fclose(in);
	if(failed) {
		perror(argv[argi]);
		exit(EXIT_FAILURE);
	}

	run();

	return EXIT_SUCCESS;
}
