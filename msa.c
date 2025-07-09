#include <stdio.h>
static void license(void) {
	puts("MIT License");
	puts("");
	puts("Copyright (c) 2025 Tristan Styles");
	puts("");
	puts("Permission is hereby granted, free of charge, to any person obtaining a copy");
	puts("of this software and associated documentation files (the \"Software\"), to deal");
	puts("in the Software without restriction, including without limitation the rights");
	puts("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
	puts("copies of the Software, and to permit persons to whom the Software is");
	puts("furnished to do so, subject to the following conditions:");
	puts("");
	puts("The above copyright notice and this permission notice shall be included in all");
	puts("copies or substantial portions of the Software.");
	puts("");
	puts("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	puts("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	puts("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
	puts("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	puts("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	puts("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	puts("SOFTWARE.");
}
#ifndef VERSION
#	define VERSION  3.2.0
#endif
//
// Build with https://github.com/stytri/m
//
// ::compile
// :+  $CC $CFLAGS $XFLAGS $SMALL-BINARY
// :+      -o $+^ $"* $"!
//
// ::debug
// :+  $CC $CFLAGS
// :+      -Og -g -DDEBUG_$: -o $^-$+: $"!
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
// ::windir?WINFLAGS
// :+      -D__USE_MINGW_ANSI_STDIO=1
//
// ::CSTD!CSTD
// :+      -std=c23
//
// ::INCLUDE!INCLUDE
// :+      -I ../inc
//
static char const *getfilename(char const *cs, int *lenp);
static void usage(char *arg0, FILE *out);
static void version(void) {
#define VERSION__STR(VERSION__STR__version)  #VERSION__STR__version
#define VERSIONTOSTR(VERSIONTOSTR__version)  VERSION__STR(VERSIONTOSTR__version)
	puts("## Version "VERSIONTOSTR(VERSION));
}
static void readme(char *arg0) {
	int         n;
	char const *file = getfilename(arg0, &n);
	printf("# %.*s\n", n, file);
	puts("");
	version();
	puts("");
	puts("## Command Line");
	puts("");
	puts("```");
	usage(arg0, stdout);
	puts("```");
	puts("");
	puts("## Description");
	puts("");
	puts("**msa** provides a basic assembler to test the instruction sets of virtual machines.");
	puts("");
	puts("**msa** does this by parsing assembler source files and creating a pattern string, which it then looks up in a hash table; it then executes an associated expression to output binary data. For patterns with a target address, another expression can be run as a linker.");
	puts("");
	puts("The patterns and their expressions are defined in `.msa` files.");
	puts("");
	puts("**N.B.** in the following, the term **byte** is the minimum addressable unit of memory of the virtual machine.");
	puts("");
	puts("## .msa files");
	puts("");
	puts("`.msa` files are recognised by the presence of the substring/extension `.msa` in the file name.");
	puts("");
	puts("The following characters have special meaning in `.msa` files: `#` `;` `{` `}`");
	puts("");
	puts("### comments");
	puts("");
	puts("Comments are introduced by the `;` character, and continue to the end of line.");
	puts("");
	puts("### line directives");
	puts("");
	puts("C preprocessor `#line` directives are recognised. Other lines with an initial `#` character are ignored.");
	puts("");
	puts("### directives");
	puts("");
	puts("**msa** directives are enclosed by `{` and `}`. There are three types of directives: those that configure the assembler, those that define reserved identifiers; e.g instruction and register names, and those that define functions.");
	puts("");
	puts("#### configuration directives");
	puts("");
	puts("These directives are identified by a initial `$`:");
	puts("");
	puts("`$BYTEBITS=` _bits_");
	puts("\n&emsp;where _bits_ is one of `8` `16` `32` `64`; default is `8`.");
	puts("");
	puts("`$BYTEFORMAT=` _format_");
	puts("\n&emsp;where _format_ is one of `OCTAL` `DECIMAL` `HEXADECIMAL`");
	puts("\n&emsp;used when printing numbers in listings; default is `HEXADECIMAL`.");
	puts("");
	puts("`$ADDRESS=` _replacement_");
	puts("\n&emsp;where _replacement_ is a sequence of graphical characters excluding '}'.");
	puts("\n&emsp;when an address (i.e. undeclared symbol - see **identifier directives** below) is encountered in the assembly source, its is replaced by _replacement_ in the pattern.");
	puts("\n&emsp;default is `%a`");
	puts("");
	puts("`$INTEGER=` _replacement_");
	puts("\n&emsp;where _replacement_ is a sequence of graphical characters excluding '}'.");
	puts("\n&emsp;when an integer value is encountered in the assembly source, its is replaced by _replacement_ in the pattern.");
	puts("\n&emsp;default is `%i`");
	puts("");
	puts("`$SEGMENTS=` _size_ [`,` `FIXED`]");
	puts("\n&emsp;where _size_ is the size of a segment in **byte**s; ISO/IEC suffixes are recognised (e.g. `Ki`) for units of 1024 **byte**s, as well as the vernacular `b` suffix (e.g. `KB`).");
	puts("\n&emsp;if segments are to be enabled, this directive _must_ be present _before_ assembler instructions are processed.");
	puts("\n&emsp;the optional `FIXED` argument forces the use of fixed size segments.");
	puts("");
	puts("`$MEMORY=` _size_");
	puts("\n&emsp;where _size_ is the size of memory in **byte**s; ISO/IEC suffixes are recognised (e.g. `Ki`) for units of 1024 **byte**s, as well as the vernacular `b` suffix (e.g. `KB`).");
	puts("\n&emsp;this directive _must_ be present _before_ assembler instructions are processed.");
	puts("");
	puts("`$NOCASE`");
	puts("\n&emsp;ignore the character case of symbols.");
	puts("");
	puts("#### identifier directives");
	puts("");
	puts("Identifiers consist of the characters `_` and `$`, the letters `A` to `Z` and `a` to `z`, and the digits `0` to `9` which _may not_ be the initial character.");
	puts("");
	puts("`{` _identifier_ `}`");
	puts("\n&emsp;defines a keyword, the _identifier_ is retained in the pattern, and _no_ field assignment is made.");
	puts("");
	puts("`{` _identifier_ `:` _replacement_ `=` _value_ `}`");
	puts("\n&emsp;defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and _value_ is assigned to the next available field. Where _replacement_ is `$`, the _replacement_ is the same as _identifier_.");
	puts("");
	puts("`{` _identifier_ `:` _replacement_ `}`");
	puts("\n&emsp;defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and a sequentially calaculated value is assigned to the next available field. Where _replacement_ is `$`, the _replacement_ is the same as _identifier_.");
	puts("");
	puts("#### set directives");
	puts("");
	puts("`{` _identifier_ `#` _replacement_ [ `=` _value_ ] `}`");
	puts("\n&emsp;defines a common _replacement_ for a *set* of _identifiers_; when processing an **identifier directive**, the _replacement_ field is first look for in the **set** table and the _replacement_ from the **set** member is used instead.");
	puts("\n&emsp;**set**s maintain independant enumeration values for identifier value assignment.");
	puts("\n&emsp;**set** member data can be exported to a C header file via the `-s` command line option:");
	puts("\n&emsp;\tfor each **set** member as a `#define` _set-identifier_`_`_member-identifier_ _member-value_");
	puts("\n&emsp;\tand an array _set-identifier_`_name` of _member-identifier_ strings.");
	puts("");
	puts("#### function directives");
	puts("");
	puts("`{` _identifier_ `{` _expression_ `}` `}`");
	puts("\n&emsp;_expression_ is evaluated when _identifier_ is encountered when evaluating an expression.");
	puts("");
	puts("### patterns");
	puts("");
	puts("_patterns_ are a sequence of graphical characters excluding the special characters mentioned above.");
	puts("");
	puts("_pattern_ `{` _expression_ `}` [ `:` `{` _link-expression_ `}` ]");
	puts("\n&emsp;_expression_ is evaluated whenever _pattern_ is matched in an assembly source file.");
	puts("\n&emsp;_link-expression_ is an optional expression to be evaluated when resolving symbol addresses; it will not be evaluated if the prior evaluation of _expression_ returns a zero value. If _link-expression_ is to be evaluated, the values of the field variants prior to the execution of _expression_ are preserved.");
	puts("");
	puts("### expressions");
	puts("");
	puts("Expressions are enclosed by a `'{' ... '}' pair.");
	puts("");
	puts("#### constants");
	puts("");
	puts("##### named constants");
	puts("");
	puts("`TRUE` or `true`");
	puts("\n&emsp;returns `1`");
	puts("");
	puts("`FALSE` or `false`");
	puts("\n&emsp;returns `0`");
	puts("");
	puts("##### integers");
	puts("");
	puts("C style representation.");
	puts("");
	puts("#### variants");
	puts("");
	puts("Variants are an initial `$` or `@`, followed by an integer number; `$` variants are used to directly access a value, '@' variants are used when accessing an address value via a symbol.");
	puts("");
	puts("Although not implicitly asssigned, the `$0` variant is intended for use as the current address.");
	puts("");
	puts("When parsing instructions, operand values are sequentially assigned to variants starting at `$1`.");
	puts("");
	puts("#### operators");
	puts("");
	puts("Operators in order of increasing precedence are:");
	puts("");
	puts("##### sequence");
	puts("");
	puts("_left_ `,` _right_");
	puts("\n&emsp;divides an expression into sub expressions that are evaluated in left-to-right order.");
	puts("");
	puts("##### assignment");
	puts("");
	puts("_address_ `@=` _value_");
	puts("\n&emsp;is the **emit** operator; it writes a single **byte** from the _value_ sub-expression to the memory at the _address_ sub-expression.");
	puts("");
	puts("_variant_ `=` _value_");
	puts("\n&emsp;assigns the _value_ sub-expression to a _variant_.");
	puts("");
	puts("##### condition");
	puts("");
	puts("_condition_ `?` _subsequent_ [ '!' _alternate_ ]");
	puts("\n&emsp;evaluates the _condition_ expresssion and if the result is non-zero, evaluates the _subsquent_ expression; if it is zero, then the optional _alternate_ expression is evaluated.");
	puts("");
	puts("##### boolean");
	puts("");
	puts("_left_ `||` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `0` if both results are zero, `1` otherwise.");
	puts("");
	puts("_left_ `&&` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if both results are non-zero, `0` otherwise.");
	puts("");
	puts("##### relation");
	puts("");
	puts("_left_ `==` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if the results are equal, `0` otherwise.");
	puts("");
	puts("_left_ `!=` _right_");
	puts("_left_ `<>` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if the results are not equal, `0` otherwise.");
	puts("");
	puts("_left_ `<` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is less than _right_, `0` otherwise.");
	puts("");
	puts("_left_ `<=` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is less than or equal to _right_, `0` otherwise.");
	puts("");
	puts("_left_ `>` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is more than _right_, `0` otherwise.");
	puts("");
	puts("_left_ `>=` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is more than or equal to _right_, `0` otherwise.");
	puts("");
	puts("##### bitwise");
	puts("");
	puts("_left_ `|` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the bitwise OR of the results.");
	puts("");
	puts("_left_ `&` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the bitwise AND of the results.");
	puts("");
	puts("_left_ `^` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the bitwise XOR of the results.");
	puts("");
	puts("##### additive");
	puts("");
	puts("_left_ `+` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the sum of the results.");
	puts("");
	puts("_left_ `-` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the result of subtracting _right_ from _left_.");
	puts("");
	puts("##### multiplicative");
	puts("");
	puts("_left_ `*` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the arithmetic product of the results.");
	puts("");
	puts("_left_ `/` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the quotient resulting from the division of _left_ by _right_.");
	puts("");
	puts("_left_ `%` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the remainder resulting from the division of _left_ by _right_.");
	puts("");
	puts("##### exponentive");
	puts("");
	puts("_left_ `<<` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise shift _left_ left by _right_.");
	puts("");
	puts("_left_ `>>` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise shift _left_ right by _right_.");
	puts("");
	puts("_left_ `<<>` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise rotation _left_ left by _right_.");
	puts("");
	puts("_left_ `<>>` _right_");
	puts("\n&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise rotation _left_ right by _right_.");
	puts("");
	puts("##### unary prefix");
	puts("");
	puts("`=@` _address_");
	puts("\n&emsp;evaluates _address_ expression and returns the byte from memory at the resulting _address_.");
	puts("");
	puts("`$&` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns the bitwise and with a global _tag_ value.");
	puts("");
	puts("`$=` _value_");
	puts("\n&emsp;evaluates _value_ expression and assigns to a global _tag_ value.");
	puts("");
	puts("`?` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns '1' if the result is non-zero, `0` otherwise.");
	puts("");
	puts("`!` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns '1' if the result is zero, `0` otherwise.");
	puts("");
	puts("`|` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns the position of the most-siginificant bit (`1` for lowest bit), or `0` if no bit set.");
	puts("");
	puts("`~` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns the bitwise complement.");
	puts("");
	puts("`-` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns the negation of the result.");
	puts("");
	puts("`+` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns its result.");
	puts("");
	puts("##### unary suffix");
	puts("");
	puts("_variant_ `++`");
	puts("\n&emsp;increments the value of _variant_, returns the pre-increment value.");
	puts("");
	puts("_variant_ `--`");
	puts("\n&emsp;decrements the value of _variant_, returns the pre-decrement value.");
	puts("");
	puts("##### primary");
	puts("");
	puts("`(` _expression_ `)`");
	puts("\n&emsp;evaluates and returns the result of _expression_.");
	puts("");
	puts("_function_");
	puts("\n&emsp;The defined _function_ expression is evaluated.");
	puts("");
	puts("_identifier_");
	puts("\n&emsp;The value assigned to (non-function) _identifier_ is returned.");
	puts("");
	puts("`INVALID`");
	puts("\n&emsp;outputs an invalid operand error message.");
	puts("\n&emsp;returns `0`.");
	puts("");
	puts("## assembly source files");
	puts("");
	puts("The following characters have special meaning in assembler files: `#` `;` `,`.");
	puts("`#` and `;` are as above, and `,` is used to separate multiple instructions on a single line.");
	puts("");
	puts("Source lines are parsed, spaces are elided, symbols and constants are replaced by their defined replacement strings, stopping at the `,` separator, comments, or end-of-line. The resulting pattern is then looked up in the pattern table and if a match is found, the corresponding expression is evaluated.");
	puts("");
	puts("## segfile");
	puts("");
	puts("With version 3.0.0 `segfile.h` was introduced as a method writing and reading a file as a sequence of segments; this header should be copied/moved to a common include directory.");
	puts("");
	puts("When enabled via the command line, segments are created automatically by **msa**; they have no attributes other than addresss and length.");
	puts("* with the `-g` command line option, segments are dynamically expanded and merged.");
	puts("* with the `-G` command line option, segments have a fixed size.");
	puts("");
	puts("## symfile");
	puts("");
	puts("With version 2.0.0 `symfile.h` was introduced as a method of passing symbol information to programs compiled with **msa**; this header should be copied/moved to a common include directory.");
	puts("");
	puts("The current version of symfile (0) only supports symbol name, information (lower 32 bits of the global _tag_), and address, although provision is made for adding more data.");
	puts("");
	puts("## Building");
	puts("");
	puts("Uses [HOL](https://github.com/stytri/hol) and [defer](https://github.com/stytri/defer).");
	puts("");
	puts("Compile with [m](https://github.com/stytri/m).");
	puts("");
}

//------------------------------------------------------------------------------

#include <hol/holibc.h>  // https://github.com/stytri/hol
#include <defer.h>       // https://github.com/stytri/defer
#include <symfile.h>
#include "config.h"
#include "segment.h"
#include "symbol.h"
#include "string.h"
#include "value.h"
#include "files.h"
#include "eval.h"

//------------------------------------------------------------------------------

static char const *symfile    = "";
static char const *setfile    = "";
static char const *outfile    = "";
static char const *infile     = "";
static int         infile_len = 0;
static size_t      lineno     = 1;

static void report_source_error(char const *fmt, ...) {
	va_list val;
	va_start(val, fmt);

	fprintf(stderr, "\n%.*s:%zu:", infile_len, infile, lineno);
	vfprintf(stderr, fmt, val);
	fputc('\n', stderr);
	fflush(stderr);

	va_end(val);
	return;
}

static void report_address_out_of_range(uint64_t a) {
	report_source_error("address %"PRIu64" out of range", a);
}

static FILE *listout = NULL;
static int   listlen = 0;

static inline void listing(char const *fmt, ...) {
	if(listout) {
		va_list val;
		va_start(val, fmt);

		int n = vfprintf(listout, fmt, val);
		if(n > 0) {
			listlen += n;
		}

		va_end(val);
	}
	return;
}

static FILE *traceout = NULL;

static inline void trace(char const *fmt, ...) {
	if(traceout) {
		va_list val;
		va_start(val, fmt);

		vfprintf(traceout, fmt, val);

		va_end(val);
	}
	return;
}

//------------------------------------------------------------------------------

static VALUE   *xref   = NULL;
static size_t   n_xref = 0;

static SYMTAB  *symtab = NULL;
static SYMTAB  *funtab = NULL;
static SYMTAB  *instab = NULL;
static SYMTAB  *settab = NULL;

static uint8_t *token   = NULL;
static size_t   n_token = 0;

//------------------------------------------------------------------------------

static int    byte_bits     = 8;
static int    byte_format   = 2;
static size_t sizeof_byte   = sizeof(uint8_t);
static size_t sizeof_memory = 0;
static void  *memory        = NULL;
static size_t maxaddr       = 0;

static bool    segments     = false;
static SEGLIST seglist      = {};

static STRING rpladdr       = STRING(2, "%a");
static STRING rplint        = STRING(2, "%i");

static uint64_t emit8(struct eval *e, uint64_t a, uint64_t v) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return v;
	}
	static char const *format[] = {
		" 0%.3"PRIo64,
		" %3"PRIu64,
		" 0x%.2"PRIX64,
	};
	listing(format[byte_format], v & UINT64_C(0xff));
	if(a > maxaddr) {
		maxaddr = a;
	}
	if(segments) {
		uint8_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m = v;
	}
	return ((uint8_t *)memory)[a] = v;
}

static uint64_t emit16(struct eval *e, uint64_t a, uint64_t v) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return v;
	}
	static char const *format[] = {
		" 0%.6"PRIo64,
		" %5"PRIu64,
		" 0x%.4"PRIX64,
	};
	listing(format[byte_format], v & UINT64_C(0xffff));
	if(a > maxaddr) {
		maxaddr = a;
	}
	if(segments) {
		uint16_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m = v;
	}
	return ((uint16_t *)memory)[a] = v;
}

static uint64_t emit32(struct eval *e, uint64_t a, uint64_t v) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return v;
	}
	static char const *format[] = {
		" 0%.11"PRIo64,
		" %10"PRIu64,
		" 0x%.8"PRIX64,
	};
	listing(format[byte_format], v & UINT64_C(0xffffffff));
	if(a > maxaddr) {
		maxaddr = a;
	}
	if(segments) {
		uint32_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m = v;
	}
	return ((uint32_t *)memory)[a] = v;
}

static uint64_t emit64(struct eval *e, uint64_t a, uint64_t v) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return v;
	}
	static char const *format[] = {
		" 0%.22"PRIo64,
		" %20"PRIu64,
		" 0x%.16"PRIX64,
	};
	listing(format[byte_format], v);
	if(a > maxaddr) {
		maxaddr = a;
	}
	if(segments) {
		uint64_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m = v;
	}
	return ((uint64_t *)memory)[a] = v;
}

static uint64_t load8(struct eval *e, uint64_t a) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return 0;
	}
	if(segments) {
		uint8_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m;
	}
	return ((uint8_t *)memory)[a];
}

static uint64_t load16(struct eval *e, uint64_t a) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return 0;
	}
	if(segments) {
		uint16_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m;
	}
	return ((uint16_t *)memory)[a];
}

static uint64_t load32(struct eval *e, uint64_t a) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return 0;
	}
	if(segments) {
		uint32_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m;
	}
	return ((uint32_t *)memory)[a];
}

static uint64_t load64(struct eval *e, uint64_t a) {
	(void)e;
	if(a > sizeof_memory) {
		report_address_out_of_range(a);
		return 0;
	}
	if(segments) {
		uint64_t *m = segment_pointer(&seglist, sizeof(*m), a);
		if(!m) {
			perror("");
			abort();
		}
		return *m;
	}
	return ((uint64_t *)memory)[a];
}

static uint64_t callfunc(EVAL *e, uint16_t a) {
	SYMBOL *sp = &funtab[a].sym;
	return eval_expression(sp->rpl.str, e);
}

static int getfunc(size_t n, char const s[]) {
	SYMBOL *sp = symbol_lookup(N_FUNCTIONS, funtab, STRING(n, s));
	if(sp) {
		SYMTAB *stp = (SYMTAB *)((char *)sp - offsetof(SYMTAB, sym));
		return  stp - funtab;
	}
	return -1;
}

static int getconst(size_t n, char const s[], uint64_t *p) {
	SYMBOL *sp = symbol_lookup(N_SYMBOLS, symtab, STRING(n, s));
	if(sp) {
		*p = sp->val.u;
		return 0;
	}
	return -1;
}

static uint64_t tag(bool set, uint64_t u) {
	static uint64_t v = 0;
	return set ? (v = u) : (v & u);
}

static EVAL env = {
	.trace = trace,
	.print = report_source_error,
	.emit  = emit8,
	.load  = load8,
	.func  = callfunc,
	.tag   = tag,
};

static int set_byte_bits(int n) {
#define SET_BYTE_BITS(Byte_Bits) \
	case Byte_Bits: \
		byte_bits   = Byte_Bits; \
		sizeof_byte = sizeof(uint##Byte_Bits##_t); \
		env.emit    = emit##Byte_Bits; \
		env.load    = load##Byte_Bits; \
		return 1;
	switch(n) {
	SET_BYTE_BITS(8)
	SET_BYTE_BITS(16)
	SET_BYTE_BITS(32)
	SET_BYTE_BITS(64)
	}
	return 0;
#undef SET_BYTE_BITS
}

//------------------------------------------------------------------------------

static inline char const *skipspace(char const *cs) {
	for(int c; (c = *cs) && (c != '\n') && !isgraph(c); cs++)
		;
	return cs;
}

static inline size_t trimspace(char const *cs, size_t n) {
	while((n > 0) && !isgraph(cs[n-1])) {
		n--;
	}
	return n;
}

static STRING compile_expression(void *p) {
	STRING expr = eval_tokenize(p, evals_getc, evals_ungetc, getfunc, getconst, '}',
		STRING((N_TOKENS - 1) - n_token,
			&token[n_token]
		),
		&lineno
	);
	n_token += expr.len;
	int e = eval_verify_expression(expr.str);
	if(e) {
		report_source_error(eval_error_message(e));
		return STRING(0, NULL);
	}
	return expr;
}

static char const *process_directive(char const *cs) {
	static unsigned long long next_val = 0;
	STRING sym;
	STRING rpl;
	VALUE  val;
	size_t n;
	char  *s;
	sym.str = cs = skipspace(cs);
	n       = strcspn(cs, "{#:=}");
	sym.len = trimspace(cs, n);
	if(cs[n] == '{') {
		val = VALUE(p, NULL, 0);
		cs += n + 1;
		rpl = compile_expression(&cs);
		n   = strcspn(cs, "}");
		cs += n;
		if(!symbol_intern(N_FUNCTIONS, funtab, ('{' << 8) | '}', 0, sym, rpl, val)) {
			report_source_error("too many functions");
		}
	} else {
		int type = '=';
		if(cs[n] == '#') {
			type = '#';
			cs += n + 1;
			rpl.str = cs;
			rpl.len = n = strcspn(cs, "=}");
			if(cs[n] == '=') {
				cs += n + 1;
				val = strtoval(cs, &s);
				n = strcspn(cs = s, "}");
			} else {
				val = VALUE(u, 0);
			}
			cs += n;
			val = (VALUE){ .type = val.u, .p = NULL };
			if(!symbol_intern(N_SETS, settab, type, 0, sym, rpl, val)) {
				report_source_error("too many sets");
			}
		} else {
			SYMBOL *set = NULL;
			if(cs[n] == ':') {
				type = ':';
				cs += n + 1;
				rpl.str = cs;
				rpl.len = n = strcspn(cs, "=}");
				if((rpl.len > 1) || memcmp(rpl.str, "$", 1)) {
					set = symbol_lookup(N_SETS, settab, rpl);
					if(set) {
						type = ';';
						rpl = set->rpl;
					}
				} else {
					rpl = sym;
				}
			} else {
				rpl = sym;
			}
			if(cs[n] == '=') {
				cs += n + 1;
				val = strtoval(cs, &s);
				n = strcspn(cs = s, "}");
				if(set) {
					set->val.type = val.u + 1;
				}
			} else if(set) {
				val = VALUE(u, set->val.type);
				set->val.type = val.u + 1;
			} else {
				val = VALUE(u, next_val);
				next_val = val.u + 1;
			}
			cs += n;
			SYMBOL *sp = symbol_intern(N_SYMBOLS, symtab, type, 0, sym, rpl, val);
			if(!sp) {
				report_source_error("too many symbols");
			} else if(set) {
				VALUE *valp = (void *)set->val.p;
				if(!valp) {
					valp = malloc(sizeof(*valp));
					if(!valp) {
						perror("");
						abort();
					}
					valp->type = 0;
					valp->p = NULL;
					set->val.p = valp;
				}
				n = valp->type;
				SYMBOL **spp = realloc((void *)valp->p, (n + 1) * sizeof(sp));
				if(!spp) {
					perror("");
					abort();
				}
				spp[n] = sp;
				valp->p = spp;
				valp->type = n + 1;
			}
		}
	}
	return cs;
}

static char const *skiptopattern(char const *cs) {
	int c;
	for(; (c = *cs) && (c != '\n'); cs++) {
		if(c == '{') {
			return cs;
		}
	}
	return NULL;
}

static char const *define_instruction(char const *cs) {
	char const *ct = skipspace(cs);
	char const *cr = skiptopattern(ct);
	if(!cr) {
		report_source_error("missing instruction expression");
		return NULL;
	}
	cs = cr + 1;
	STRING rpl = compile_expression(&cs);
	if(rpl.len == 0) {
		return NULL;
	}
	for(; (cr > ct) && isblank(*(cr - 1)); cr--)
		;
	STRING sym = STRING(cr - ct, ct);
	STRING ltx = STRING(0, NULL);
	if(*(ct = cs) == ':') {
		ct++;
		if(*ct == '{') {
			ct++;
		}
		cs = ct;
		ltx = compile_expression(&cs);
		if(ltx.len == 0) {
			return NULL;
		}
	}
	VALUE val = VALUE(p, ltx.str);
	if(!symbol_intern(N_INSTRUCTIONS, instab, '$', 0, sym, rpl, val)) {
		report_source_error("too many instructions");
	}
	return cs;
}

static char const *compile_instruction(char const *cs) {
	static size_t bufsiz = 0;
	static char  *buf    = NULL;
	char    *t  = buf, *end = buf + bufsiz;
#define PUSHC(PUSHC__c) \
	do { \
		if(t == end) { \
			size_t n = t - buf; \
			size_t z = bufsiz + BUFSIZ; \
			char  *p = realloc(buf, z); \
			if(!p) { \
				perror(); \
				abort(); \
			} \
			buf = p, bufsiz = z - 1; \
			end = buf + bufsiz; \
			t   = buf + n; \
		} \
		*t++ = (PUSHC__c); \
	} while(0)
	unsigned nv = 1;
	VALUE   *xr = NULL;
	size_t   o_xref = n_xref;
	size_t   i;
	char    *s;
	char const *cy = NULL;
	for(int c; (c = *cs) && (c != '\n');) {
		if(!isgraph(c)) {
			cs++;
			continue;
		}
		cy = cy ? cy : cs;
		if((c == '_') || (c == '$') || isalpha(c)) {
			char const *ct = cs;
			do {
				c = *++cs;
			} while((c == '_') || (c == '$') || isalnum(c))
				;
			SYMBOL *sp = symbol_intern(N_SYMBOLS, symtab, '@', tag(false, -1), STRING(cs - ct, ct), rpladdr, VALUE(u, 0));
			if(!sp) {
				report_source_error("too many symbols");
				return NULL;
			}
			switch(sp->type) {
			case '@':
				if(!xr) {
					if(n_xref >= (N_REFERENCES - (2 + nv))) {
						report_source_error("too many references");
						return NULL;
					}
					xr = &xref[n_xref++];
					xr->type = '@';
					xr->p    = NULL;
					xref[n_xref].type = '?';
					xref[n_xref].u = tag(false, -1);
					n_xref++;
					for(size_t i = 0; i < nv; i++) {
						xref[n_xref++] = env.v[i];
						xr->type += 1 << 8;
					}
				}
				if(nv >= N_EVAL_VARIANTS) {
					report_source_error("too many operands");
					return NULL;
				}
				if(n_xref >= N_REFERENCES) {
					report_source_error("too many references");
					return NULL;
				}
				i = nv++;
				env.v[i] = VALUE(u, (uint64_t)&sp->val.u);
				xref[n_xref++] = env.v[i];
				xr->type += 1 << 8;
				break;
			case ':': case ';':
				i = nv++;
				env.v[i] = sp->val;
				if(xr) {
					if(n_xref >= N_REFERENCES) {
						report_source_error("too many references");
						return NULL;
					}
					xref[n_xref++] = env.v[i];
					xr->type += 1 << 8;
				}
				break;
			}
			for(i = 0; i < sp->rpl.len; i++) {
				PUSHC(((char *)sp->rpl.str)[i]);
			}
			continue;
		}
		if(isdigit(c) || (c == '\'')) {
			if(nv >= N_EVAL_VARIANTS) {
				report_source_error("too many operands");
				return NULL;
			}
			i = nv++;
			env.v[i] = strtoval(cs, &s);
			if(xr) {
				if(n_xref >= N_REFERENCES) {
					report_source_error("too many references");
					return NULL;
				}
				xref[n_xref++] = env.v[i];
				xr->type += 1 << 8;
			}
			switch(env.v[i].type) {
			case 'u':
				for(i = 0; i < rplint.len; i++) {
					PUSHC(((char *)rplint.str)[i]);
				}
				cs = s;
				continue;
			default:
				report_source_error("invalid operand: %.*s", (int)(s - cs), cs);
				cs = s;
				continue;
			}
		}
		if(c == ';') {
			do {
				c = *++cs;
			} while(c && (c != '\n'))
				;
			break;
		}
		if(c == ',') {
			cs++;
			break;
		}
		PUSHC(c);
		cs++;
	}
	*t = '\0';
	if(t != buf) {
		SYMBOL *sp = symbol_lookup(N_INSTRUCTIONS, instab, STRING(t - buf, buf));
		if(!sp) {
			report_source_error("invalid instruction: %.*s", (int)(t - buf), buf);
		} else {
			int n = cs - cy;
			while((n > 0) && !isgraph(cy[n-1])) {
				n--;
			}
			listlen = 0;
			listing("%6"PRIu64":  ", env.v[0].u);
			uint64_t l = eval_expression(sp->rpl.str, &env);
			for(listlen = (listlen < 40) ? 40 - listlen : 0; listlen > 8; listlen -= 8) {
				fputs("        ", listout);
			}
			if(listlen > 0) {
				fputs("        " + (8 - listlen), listout);
			}
			listing("  ; %.*s  ;; %.*s\n", n, cy, (int)sp->sym.len, sp->sym.str);
			if(xr) {
				if(l) {
					xr->p = sp->val.p;
				} else {
					n_xref = o_xref;
				}
			}
		}
	}
	return cs;
#undef PUSHC
}

static bool source_file_is_msa(char const *infile, size_t n) {
	for(size_t m = 0; n-- > 0; ) {
		if(infile[n] == '.') {
			if((m == 3) && (strncasecmp(&infile[n + 1], "msa", 3) == 0)) {
				return true;
			}
			m = 0;
			continue;
		}
		if(!isalpha(infile[n])) {
			m = 0;
			continue;
		}
		m++;
	}
	return false;
}

static int cmpsymbols(void const *lp, void const *rp) {
	SYMBOL const *ls = *(SYMBOL const **)lp;
	SYMBOL const *rs = *(SYMBOL const **)rp;
	size_t const  ln = ls->sym.len;
	size_t const  rn = rs->sym.len;
	size_t const  n  = (ln < rn) ? ln : rn;
	int    const  r  = strncmp(ls->sym.str, rs->sym.str, n);
	if(r == 0) {
		if(ln < rn) return -1;
		if(ln > rn) return +1;
	}
	return r;
}

//------------------------------------------------------------------------------

#ifdef __MINGW64__
int _dowildcard = -1;
#endif

static struct optget options[] = {
	{  0, "Matching String Assembler: a simple assembler for virtual machines.", NULL },
	{  0, "usage: %s [OPTION]... [FILE]...",   NULL },
	{  0, "options:",                NULL },
	{  1, "-h, --help",              "display this help and exit" },
	{  2, "    --version",           "display version and exit" },
	{  3, "    --license",           "display license and exit" },
	{  4, "    --readme",            "display readme and exit" },
	{  9, "-o, --output FILE",       "output to FILE" },
	{ 10, "-l, --listing FILE",      "write listing to FILE" },
#ifdef EVAL_TRACE
	{ 11, "-t, --trace FILE",        "write trace to FILE" },
#endif//def EVAL_TRACE
	{ 12, "-s, --set-header FILE",   "write header FILE for sets" },
	{ 13, "-y, --symbols PFIX",      "include non-set symbols, adding prefix PFIX, in set header" },
	{ 14, "-a, --symfile FILE",      "write (addressable) symbol information to FILE" },
	{ 15, "-g, --segments SIZE",     "enable segmented memory - segments are allocated in blocks of SIZE bytes." },
	{ 16, "-G, --fixed-segments SIZE","as -g, but segments are of fixed SIZE" },
};
static size_t const n_options = (sizeof(options) / sizeof(options[0]));

static void usage(char *arg0, FILE *out) {
	optuse(n_options, options, arg0, out);
}

static inline bool qerror(char const *cs) {
	perror(cs);
	return false;
}

static FILE *openout(char const *file, FILE *out) {
	if(out && (out != stdout)) {
		fclose(out);
	}
	if(strcmp(file, "-") == 0) {
		out = stdout;
	} else {
		out = fopen(file, "w");
		if(!out) {
			perror(file);
		}
	}
	return out;
}

int
main(
	int    argc,
	char **argv
) {
	bool failed = false;
	char const *prefix = NULL;

	int argi = 1;
	if(argi == argc) {
		usage(argv[0], stderr);
		return EXIT_FAILURE;
	}
	while((argi < argc) && (*argv[argi] == '-') && *(argv[argi] + 1)) {
		char const *args = argv[argi++];
		char const *argp = NULL;
		do {
			int argn   = argc - argi;
			int params = 0;
			switch(optget(n_options - 2, options + 2, &argp, args, argn, &params)) {
			case 1:
				usage(argv[0], stdout);
				return 0;
			case 2:
				version();
				return 0;
			case 3:
				license();
				return 0;
			case 4:
				readme(argv[0]);
				return 0;
			case 9:
				outfile = argv[argi];
				break;
			case 10:
				failed = failed || !(listout = openout(argv[argi], listout));
				break;
#ifdef EVAL_TRACE
			case 11:
				failed = failed || !(traceout = openout(argv[argi], traceout));
				break;
#endif//def EVAL_TRACE
			case 12:
				setfile = argv[argi];
				break;
			case 13:
				prefix = argv[argi];
				break;
			case 14:
				symfile = argv[argi];
				break;
			case 15:
				seglist.flag &= ~FIXED_SEGMENTS;
				seglist.granularity = strtozs(argv[argi], NULL, 0);
				segments = true;
				break;
			case 16:
				seglist.flag |= FIXED_SEGMENTS;
				seglist.granularity = strtozs(argv[argi], NULL, 0);
				segments = true;
				break;
			default:
				errorf("invalid option: %s", args);
				usage(argv[0], stderr);
				return EXIT_FAILURE;
			}
			argi += params;
		} while(argp)
			;
	}

	if(!failed) {
		xref   = calloc(N_REFERENCES  , sizeof(*xref));
		symtab = calloc(N_SYMBOLS     , sizeof(*symtab));
		funtab = calloc(N_FUNCTIONS   , sizeof(*funtab));
		instab = calloc(N_INSTRUCTIONS, sizeof(*instab));
		settab = calloc(N_SETS        , sizeof(*settab));
		token  = calloc(N_TOKENS      , sizeof(*token));
		if(!xref || !symtab  || !funtab || !instab || !token) {
			perror();
			abort();
		}
	}

	for(size_t i = 0; i < N_EVAL_VARIANTS; i++) {
		env.v[i] = VALUE(u, 0);
	}

	listlen = 0;
	listing("\n; LISTING\n; =======\n\n");

	lineno = 1;
	infile = argv[argi];
	infile_len = strlen(infile);
	char const *(*process)(char const *) = define_instruction;
	DEFER(char const *src = loadsourcefiles(argc - argi, &argv[argi]),
		!(failed = !src) || qerror(infile),
		(void)0
	) for(char const *cs = src;;) {
		int c = *(cs = skipspace(cs));
		if(c == ';') {
			do {
				c = *++cs;
			} while(c && (c != '\n'))
				;
		}
		if(c == '\n') {
			lineno++;
			cs++;
			continue;
		}
		if(c == '#') {
			c = *(cs = skipspace(cs + 1));
			if(strncasecmp(cs, "line", 4) == 0) {
				c = *(cs = skipspace(cs + 4));
			}
			bool line_directive = isdigit(c);
			if(line_directive) {
				char *s = (char *)cs;
				lineno = (size_t)strtoumax(cs, &s, 0);
				cs = s;
				c = *(cs = skipspace(cs));
				if(c == '"') {
					infile = cs + 1;
					do {
						c = *++cs;
					} while(c && (c != '"') && (c != '\n'))
						;
					infile_len = cs - infile;
					if(source_file_is_msa(infile, infile_len)) {
						process = define_instruction;
					} else {
						process = compile_instruction;
					}
					if(c == '"') {
						c = *++cs;
					}
				}
			}
			while(c && (c != '\n')) {
				c = *++cs;
			}
			if(line_directive) {
				if(c == '\n') cs++;
			}
			continue;
		}
		if(c == '{') {
			if(*(cs + 1) == '$') {
				char *s;
				if(strncasecmp(cs+2, "BYTEBITS=", 9) == 0) {
					int n = strtol(cs+11, &s, 10);
					cs = skipspace(s);
					if((*cs == '}') && set_byte_bits(n)) {
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "BYTEFORMAT=", 11) == 0) {
					cs = skipspace(cs+13);
					if(strncasecmp(cs,"OCTAL", 5) == 0) {
						cs = skipspace(cs + 5);
						if(*cs == '}') {
							byte_format = 0;
							cs++;
							continue;
						}
					} else if(strncasecmp(cs,"DECIMAL", 7) == 0) {
						cs = skipspace(cs+7);
						if(*cs == '}') {
							byte_format = 1;
							cs++;
							continue;
						}
					} else if(strncasecmp(cs,"HEXADECIMAL", 11) == 0) {
						cs = skipspace(cs+11);
						if(*cs == '}') {
							byte_format = 2;
							cs++;
							continue;
						}
					}
				} else if(strncasecmp(cs+2, "ADDRESS=", 8) == 0) {
					s = (char *)skipspace(cs+10);
					while(isgraph(*cs) && (*cs != '}')) {
						cs++;
					}
					if(*cs == '}') {
						rpladdr = STRING(cs - s, s);
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "INTEGER=", 8) == 0) {
					s = (char *)skipspace(cs+10);
					while(isgraph(*cs) && (*cs != '}')) {
						cs++;
					}
					if(*cs == '}') {
						rplint = STRING(cs - s, s);
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "SEGMENTS=", 9) == 0) {
					seglist.flag &= ~FIXED_SEGMENTS;
					seglist.granularity = strtozs(cs+11, &s, 10);
					cs = skipspace(s);
					if(*cs == ',') {
						cs = skipspace(cs+1);
						if(strncasecmp(cs, "FIXED", 5) == 0) {
							cs = skipspace(cs+5);
							seglist.flag |= FIXED_SEGMENTS;
						}
					}
					if(*cs == '}') {
						segments = true;
						if(memory) {
							free(memory);
							memory = NULL;
						}
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "MEMORY=", 7) == 0) {
					sizeof_memory = strtozs(cs+9, &s, 10);
					cs = skipspace(s);
					if(*cs == '}') {
						if(memory) {
							free(memory);
							memory = NULL;
						}
						if(!segments) {
							memory = calloc(sizeof_memory, sizeof_byte);
							if(!memory) {
								perror();
								abort();
							}
						}
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "NOCASE", 6) == 0) {
					s = (char *)skipspace(cs+10);
					if(*cs == '}') {
						equal = nocase_equal;
						cs++;
						continue;
					}
				}
			} else {
				failed = failed || !(cs = process_directive(cs+1));
				if(cs) {
					cs = skipspace(cs);
					if(*cs == '}') {
						cs++;
						continue;
					}
				}
			}
			do {
				c = *++cs;
			} while(c && (c != '\n'))
				;
			report_source_error("invalid directive");
			failed = true;
			continue;
		}
		if(!c) {
			break;
		}
		failed = failed || !(cs = process(cs));
	}

	if(!failed && (n_xref > 0)) {
		listlen = 0;
		listing("\n; PATCHES\n; =======\n\n");
		for(size_t i = 0; i < n_xref; ) {
			size_t  const  n = xref[i].type >> 8;
			uint8_t const *l = xref[i++].p;
			size_t         j = 0;
			for(tag(true, xref[i++].u); j < n;) {
				env.v[j++] = xref[i++];
			}
			if(l) {
				listlen = 0;
				listing("%6"PRIu64":  ", env.v[0].u);
				eval_expression(l, &env);
				listing("\n");
			}
		}
	}
	if(!failed) {
		listlen = 0;
		listing("\n; SIZE = %zu\n\n", maxaddr + 1);
	}
	if(listout && (listout != stdout)) {
		fclose(listout);
	}

	if(!failed && *outfile) DEFER(FILE *out = fopen(outfile, "wb"),
		!(failed = !out) || qerror(outfile),
		fclose(out)
	) {
		if(segments) {
			if(segfile_write(&seglist, sizeof_byte, sizeof_memory, out) != 0) {
				failed = true;
				perror(outfile);
				break;
			}
			continue;
		}
		size_t z = fwrite(memory, sizeof_byte, maxaddr + 1, out);
		if(z != (maxaddr + 1)) {
			perror(outfile);
		}
	}

	if(!failed && *setfile) DEFER(FILE *out = fopen(setfile, "w"),
		!(failed = !out) || qerror(setfile),
		fclose(out)
	) {
		int         setnamelen = 0;
		char const *setname    = getfilename(setfile, &setnamelen);
		fprintf(out, "#ifndef %.*s__included\n", setnamelen, setname);
		fprintf(out, "#define %.*s__included 1\n\n", setnamelen, setname);
		fprintf(out, "// this is a generated file DO NOT edit\n");
		for(size_t i = 0; i < N_SETS; i++) {
			if(settab[i].ref == 0) {
				continue;
			}
			fprintf(out, "\n");
			SYMBOL const *set = &settab[i].sym;
			VALUE  const *valp = set->val.p;
			if(valp) {
				SYMBOL *sp, **spp, **end;
				size_t n = valp->type;
				for(spp = (SYMBOL **)valp->p, end = spp + n; spp < end; ) {
					sp = *spp++;
					fprintf(out, "#define %.*s_%.*s %llu\n",
						(int)set->sym.len, (char const *)set->sym.str,
						(int)sp->sym.len, (char const *)sp->sym.str,
						sp->val.u
					);
				}
				fprintf(out, "\nstatic char const *%.*s_name[] = {\n",
					(int)set->sym.len, (char const *)set->sym.str
				);
				for(spp = (SYMBOL **)valp->p, end = spp + n; spp < end; ) {
					sp = *spp++;
					fprintf(out, "\t[%llu] = \"%.*s\",\n",
						sp->val.u, (int)sp->sym.len, (char const *)sp->sym.str
					);
				}
			}
			fprintf(out, "};\n");
		}
		if(prefix) {
			fprintf(out, "\n");
			for(size_t i = 0; i < N_SYMBOLS; i++) {
				if(symtab[i].ref == 0) {
					continue;
				}
				SYMBOL *sp = &symtab[i].sym;
				if((sp->type != ':') && (sp->type != '=')) {
					continue;
				}
				fprintf(out, "#define %s%.*s %llu\n",
					prefix,
					(int)sp->sym.len, (char const *)sp->sym.str,
					sp->val.u
				);
			}
		}
		fprintf(out, "\n#endif//ndef %.*s__included\n", setnamelen, setname);
	}

	if(!failed && *symfile) DEFER(FILE *out = fopen(symfile, "wb"),
		!(failed = !out) || qerror(setfile),
		fclose(out)
	) {
		size_t n_symbols = 0;
		size_t names_len = 2;
		for(size_t i = 0; i < N_SYMBOLS; i++) {
			SYMBOL const *sp = &symtab[i].sym;
			if((symtab[i].ref == 0) || (sp->type != '@')) {
				continue;
			}
			names_len += sp->sym.len + 1;
			n_symbols++;
		}
		uint8_t         hd[16]; memset(hd, 0, sizeof(hd));
		uint32_t        ri[64]; memset(ri, 0, sizeof(ri));
		symfile_symbol *sfp   = calloc(n_symbols, sizeof(*sfp));
		SYMBOL const  **spp   = calloc(n_symbols, sizeof(*spp));
		char           *names = calloc(names_len, 1);
		if(!sfp || !spp || !names) {
			perror();
			break;
		}
		for(size_t j = 0, i = 0; i < N_SYMBOLS; i++) {
			SYMBOL const *sp = &symtab[i].sym;
			if((symtab[i].ref == 0) || (sp->type != '@')) {
				continue;
			}
			spp[j++] = sp;
		}
		qsort(spp, n_symbols, sizeof(*spp), cmpsymbols);
		int      rix = 0;
		uint32_t sfx = 0;
		uint32_t nsx = 1;
		for(size_t i = 0; i < n_symbols; i++) {
			SYMBOL const *sp = spp[i];
			symfile_append(ri, &rix, sfp, &sfx, names, &nsx,
				sp->sym.len, sp->sym.str, sp->tag & UINT32_C(0xFFFFFFFF), sp->val.u
			);
		}
		uint64_t data = 0;
		if(symfile_write(hd, ri, n_symbols, sfp, names_len, names, sizeof(data), &data, out) != 0) {
			perror(symfile);
		}
	}

	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
