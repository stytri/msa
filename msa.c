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
#	define VERSION  3.6.0
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
static char const *get_nameof_character_by_index(size_t);
static int get_named_character_by_index(size_t);
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
	puts("\n&emsp;where _replacement_ is a sequence of graphical characters excluding `}`.");
	puts("\n&emsp;when an address (i.e. undeclared symbol - see **identifier directives** below) is encountered in the assembly source, its is replaced by _replacement_ in the pattern.");
	puts("\n&emsp;default is `%a`");
	puts("");
	puts("`$INTEGER=` _replacement_");
	puts("\n&emsp;where _replacement_ is a sequence of graphical characters excluding `}`.");
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
	puts("In the following _character_, refers to a **named character**; these are:");
	int c;
	for(size_t i = 0; (c = get_named_character_by_index(i)) >= 0; i++) {
		char const *cs = get_nameof_character_by_index(i);
		if(c) {
			printf("-\t`%s` yields character `%c`\n", cs, c);
		} else {
			printf("-\t`%s` disables COMMENT and DELIMITER; LABEL is left alone.\n", cs);
		}
	}
	puts("\n&emsp;the above character names may be preceeded by `&`, and terminated by `;`, as per HTML.");
	puts("");
	puts("`$COMMENT=` _character_");
	puts("\n&emsp;in assembler source files, _character_ starts a comment (default `;`).");
	puts("\n&emsp;if COMMENT is disabled, then `#` becomes the defacto comment character (see description of [`#line`](#line-directives)).");
	puts("");
	puts("`$DELIMITER=` _character_");
	puts("\n&emsp;in assembler source files, _character_ delimits multiple instructions on a single line (default `,`).");
	puts("");
	puts("`$LABEL=` _character_");
	puts("\n&emsp;in assembler source files, _character_ indicates that the preceeding symbol is a label definition (default `:`).");
	puts("");
	puts("`$NOCASE`");
	puts("\n&emsp;ignore the character case of symbols.");
	puts("");
	puts("#### identifier directives");
	puts("");
	puts("Identifiers consist of the characters `_` and `$`, the letters `A` to `Z` and `a` to `z`, and the digits `0` to `9` which _may not_ be the initial character.");
	puts("Additionally, the back-tick ( ` ) character can be used to quote the next [non-identifier] graphical character as part of the identifier;");
	puts("**N.B.** _identifiers_ containing quoted characters are hidden from export.");
	puts("");
	puts("Identifiers may be preceeded by an _attribute_, current attributes are:");
	puts("-	`{delimiter}`  indicates that the _identifier_ is to be treated as an instruction delimiter.");
	puts("-	`{hidden}`     indicates that the _identifier_ is to be excluded from export.");
	puts("-	`{postfix}`    indicates that the **set** _identifier_ is to be appended to the _member_ _identifier_ when exported: normally it will be prepended.");
	puts("");
	puts("`{` [ _attribute_ ] _identifier_ `}`");
	puts("\n&emsp;defines a keyword, the _identifier_ is retained in the pattern, and _no_ field assignment is made.");
	puts("");
	puts("`{` [ _attribute_ ] _identifier_ `:` _replacement_ `=` _expression_ `}`");
	puts("\n&emsp;defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and value of _expression_ is assigned to the next available field. Where _replacement_ is `$`, the _replacement_ is the same as _identifier_.");
	puts("\n&emsp;**N.B.** _expression_ is evaluated immediately.");
	puts("");
	puts("`{` [ _attribute_ ] _identifier_ `:` _replacement_ `}`");
	puts("\n&emsp;defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and a sequentially calaculated value is assigned to the next available field. Where _replacement_ is `$`, the _replacement_ is the same as _identifier_.");
	puts("");
	puts("#### set directives");
	puts("");
	puts("`{` _identifier_ `#` _replacement_ [ `=` _expression_ ] `}`");
	puts("\n&emsp;defines a common _replacement_ for a *set* of _identifiers_; when processing an **identifier directive**, the _replacement_ field is first look for in the **set** table and the _replacement_ from the **set** member is used instead.");
	puts("\n&emsp;**set**s maintain independant enumeration values for identifier value assignment. Additionaly, the members of the **set** inherit the attributes of the **set**.");
	puts("\n&emsp;**set** member data can be exported to a C header file via the `-s` command line option:");
	puts("\n&emsp;\tfor each **set** member as a `#define` _set-identifier_`_`_member-identifier_ _member-value_");
	puts("\n&emsp;\tand an array _set-identifier_`_name` of _member-identifier_ strings.");
	puts("\n&emsp;**N.B.** _expression_ is evaluated immediately.");
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
	puts("Expressions are enclosed by a `{` ... `}` pair.");
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
	puts("C style representation, and simple character constants.");
	puts("");
	puts("#### variants");
	puts("");
	puts("Variants are an initial `$` or `@`, followed by an integer number; `$` variants are used to directly access a value, `@` variants are used when accessing an address value via a symbol.");
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
	puts("_condition_ `?` _subsequent_ [ `!` _alternate_ ]");
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
	puts("`$?`_variant_");
	puts("\n&emsp;where _variant_ is the variant number; returns the type identifier of the _variant_.");
	puts("");
	puts("`=@` _address_");
	puts("\n&emsp;evaluates _address_ expression and returns the byte from memory at the resulting _address_.");
	puts("");
	puts("`@&`_variant_ _value_");
	puts("\n&emsp;evaluates _value_ expression and returns the bitwise and with the symbol _variant_'s _tag_ value.");
	puts("");
	puts("`$&` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns the bitwise and with a global _tag_ value.");
	puts("");
	puts("`$=` _value_");
	puts("\n&emsp;evaluates _value_ expression and assigns to a global _tag_ value.");
	puts("");
	puts("`?` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns `1` if the result is non-zero, `0` otherwise.");
	puts("");
	puts("`!` _value_");
	puts("\n&emsp;evaluates _value_ expression and returns `1` if the result is zero, `0` otherwise.");
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
	puts("Source lines are parsed, spaces are elided, symbols and constants are replaced by their defined replacement strings, stopping at the `,` or `:` separators (the colon separator is preserved in the pattern, the comma is elided), comments, or end-of-line. The resulting pattern is then looked up in the pattern table and if a match is found, the corresponding expression is evaluated.");
	puts("");
	puts("Inside parenthesis (`(`...`)` or, `[`...`]`) delimiters and comments are inhibited, and the characters can be used as part of the pattern.");
	puts("Parenthesis handling is _very_ basic; `(` and `[` increase the nesting level,  `]` and `)` decrease it; there is no balancing or, matching of the parenthesis characters.");
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

static FILE *errout = NULL;

static void report_source_error(char const *fmt, ...) {
	va_list val;
	va_start(val, fmt);
#ifdef NDEBUG
	if(!errout) {
		errout = stderr;
	}
	fprintf(errout, "\n%.*s:%zu:", infile_len, infile, lineno);
	vfprintf(errout, fmt, val);
	fputc('\n', errout);
	fflush(errout);
#endif
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

static char delimiter_char = ',';
static char comment_char   = ';';
static char label_char     = ':';

static struct {
	char const *ct;
	int         c;
} const named_char[] = {
	{ "amp"   , '&'  },
	{ "ast"   , '*'  },
	{ "bsol"  , '\\' },
	{ "colon" , ':'  },
	{ "comma" , ','  },
	{ "commat", '@'  },
	{ "equals", '='  },
	{ "excl"  , '!'  },
	{ "hat"   , '^'  },
	{ "lbrack", '['  },
	{ "lpar"  , '('  },
	{ "lt"    , '<'  },
	{ "gt"    , '>'  },
	{ "percnt", '%'  },
	{ "period", '.'  },
	{ "quest" , '?'  },
	{ "quot"  , '"'  },
	{ "rbrack", ']'  },
	{ "rpar"  , ')'  },
	{ "semi"  , ';'  },
	{ "sol"   , '/'  },
	{ "tilde" , '~'  },
	{ "vert"  , '|'  },
	{ "none"  ,  0   },
};
static char const *get_nameof_character_by_index(size_t i) {
	if(i < (sizeof(named_char)/sizeof(*named_char))) {
		return named_char[i].ct;
	}
	return "unknown";
}
static int get_named_character_by_index(size_t i) {
	if(i < (sizeof(named_char)/sizeof(*named_char))) {
		return named_char[i].c;
	}
	return -1;
}
static int get_named_character(int n, char const *cs, int c) {
	if((n > 0) && (*cs == '&')) {
		cs++;
		n--;
	}
	if((n > 0) && (cs[n - 1] == ';')) {
		n--;
	}
	if(n > 0) for(size_t i = 0; i < (sizeof(named_char)/sizeof(*named_char)); i++) {
		int m = strlen(named_char[i].ct);
		if((n == m) && (strncmp(cs, named_char[i].ct, m) == 0)) {
			return (named_char[i].c > 0) ? named_char[i].c : c;
		}
	}
	return c;
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

static uint64_t emit8(EVAL *e, uint64_t a, uint64_t v) {
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

static uint64_t emit16(EVAL *e, uint64_t a, uint64_t v) {
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

static uint64_t emit32(EVAL *e, uint64_t a, uint64_t v) {
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

static uint64_t emit64(EVAL *e, uint64_t a, uint64_t v) {
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

static uint64_t load8(EVAL *e, uint64_t a) {
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

static uint64_t load16(EVAL *e, uint64_t a) {
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

static uint64_t load32(EVAL *e, uint64_t a) {
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

static uint64_t load64(EVAL *e, uint64_t a) {
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

static uint64_t *symptr(uint64_t a) {
	SYMBOL *sp = (SYMBOL *)a;
	return &sp->val.u;
}

static uint64_t symtag(uint64_t a, uint64_t u) {
	SYMBOL *sp = (SYMBOL *)a;
	return sp->tag & u;
}

static uint64_t tag(bool set, uint64_t u) {
	static uint64_t v = 0;
	return set ? (v = u) : (v & u);
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

static EVAL env = {
	.trace = trace,
	.print = report_source_error,
	.emit  = emit8,
	.load  = load8,
	.func  = callfunc,
	.symp  = symptr,
	.stag  = symtag,
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
	STRING xpr;
	VALUE  val;
	uint64_t st = 0;
	size_t n = 0;
	for(cs = skipspace(cs); *cs == '{'; ) {
		n = strcspn(cs, "}");
		n += (cs[n] == '}');
		if(strncmp(cs, "{hidden}", n) == 0) {
			st |= HIDDEN_SYMBOL;
		} else if(strncmp(cs, "{delimiter}", n) == 0) {
			st |= DELIMITER_SYMBOL;
		} else if(strncmp(cs, "{postfix}", n) == 0) {
			st |= POSTFIX_SYMBOL;
		} else {
			report_source_error("unknown attribute");
		}
		cs = skipspace(cs + n);
		n = 0;
	}
	sym.len = 0;
	sym.str = cs;
	for(int quote = 0, c = *cs;
		(quote && isgraph(c)) || (c == '`') || (c == '_') || (c == '$') || isalnum(c);
		c = cs[sym.len]
	) {
		quote = !quote && (c == '`');
		st |= quote ? HIDDEN_SYMBOL : 0;
		sym.len++;
	}
	if(sym.len == 0) {
		report_source_error("missing identifier");
	}
	cs = skipspace(cs + sym.len);
	if(cs[n] == '{') {
		val = VALUE(p, NULL, 0);
		cs += n + 1;
		rpl = compile_expression(&cs);
		n   = strcspn(cs, "}");
		cs += n;
		if((sym.len > 0)
			&& !symbol_intern(N_FUNCTIONS, funtab, ('{' << 8) | '}', st, sym, rpl, val)
		) {
			report_source_error("too many functions");
		}
	} else {
		int type = '=';
		if(cs[n] == '#') {
			type = '#';
			cs += n + 1;
			rpl.str = cs;
			rpl.len = n = strcspn(cs, "=}");
			val = VALUE(u, 0);
			if(cs[n] == '=') {
				cs += n + 1;
				xpr = compile_expression(&cs);
				val.u = eval_expression(xpr.str, &env);
				cs -= (xpr.len > 0); // restore terminating '}'
				n = 0;
			}
			cs += n;
			val = (VALUE){ .type = val.u, .p = NULL };
			if((sym.len > 0)
				&& !symbol_intern(N_SETS, settab, type, st, sym, rpl, val)
			) {
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
						st |= set->tag;
					}
				} else {
					rpl = sym;
				}
			} else {
				rpl = sym;
			}
			val = VALUE(u, 0);
			if(cs[n] == '=') {
				cs += n + 1;
				xpr = compile_expression(&cs);
				val.u = eval_expression(xpr.str, &env);
				cs -= (xpr.len > 0); // restore terminating '}'
				n = 0;
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
			if(sym.len > 0) {
				SYMBOL *sp = symbol_intern(N_SYMBOLS, symtab, type, st, sym, rpl, val);
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
		report_source_error("invalid instruction expression");
		return cs;
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
			report_source_error("invalid link expression");
			return cs;
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
	unsigned paren = 0;
	size_t   i;
	char    *s;
	char const *cy = NULL;
	for(int c; (c = *cs) && (c != '\n');) {
		if(!isgraph(c)) {
			cs++;
			continue;
		}
		cy = cy ? cy : cs;
		if((c == '`') || (c == '_') || (c == '$') || isalpha(c)) {
			char const *ct = cs;
			bool quote = false, hidden = false;
			do {
				quote = !quote && (c == '`');
				hidden |= quote;
				c = *++cs;
			} while((quote && isgraph(c)) || (c == '`') || (c == '_') || (c == '$') || isalnum(c))
				;
			uint64_t st = tag(false, -1) | (hidden ? HIDDEN_SYMBOL : 0);
			SYMBOL *sp = symbol_intern(N_SYMBOLS, symtab, '@', st, STRING(cs - ct, ct), rpladdr, VALUE(u, 0));
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
					xr->type = 'X';
					xr->p    = NULL;
					xref[n_xref].type = '?';
					xref[n_xref].u = st;
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
				env.v[i] = (VALUE){ .type = '@', .u = (uint64_t)sp };
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
			if(sp->tag & DELIMITER_SYMBOL) {
				if(!paren) {
					break;
				}
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
		if(comment_char && (c == comment_char)) {
			if(!paren) {
				do {
					c = *++cs;
				} while(c && (c != '\n'))
					;
				break;
			}
		}
		if(delimiter_char && (c == delimiter_char)) {
			cs++;
			if(!paren) {
				break;
			}
		}
		PUSHC(c);
		cs++;
		if((c == '(') || (c == '[')) {
			paren++;
			continue;
		}
		if((c == ')') || (c == ']')) {
			paren--;
			continue;
		}
		if(c == label_char) {
			if(!paren) {
				break;
			}
		}
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

static bool process_files(int argi, int argc, char **argv) {
	bool failed = false;

	lineno = 1;
	infile = argv[argi];
	infile_len = strlen(infile);
	char const *(*process)(char const *) = define_instruction;
	int comment = ';';
	DEFER(char const *src = loadsourcefiles(argc - argi, &argv[argi]),
		!(failed = !src) || qerror(infile),
		(void)0
	) for(char const *cs = src;;) {
		int c = *(cs = skipspace(cs));
		if(comment && (c == comment)) {
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
				cs = s ? s : cs;
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
						comment = ';';
					} else {
						process = compile_instruction;
						comment = comment_char;
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
					cs = s ? skipspace(s) : cs;
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
					for(cs = s; isgraph(*cs) && (*cs != '}');) {
						cs++;
					}
					if(*cs == '}') {
						rpladdr = STRING(cs - s, s);
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "INTEGER=", 8) == 0) {
					s = (char *)skipspace(cs+10);
					for(cs = s; isgraph(*cs) && (*cs != '}');) {
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
					cs = s ? skipspace(s) : cs;
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
				} else if(strncasecmp(cs+2, "COMMENT=", 8) == 0) {
					s = (char *)skipspace(cs+10);
					for(cs = s; isgraph(*cs) && (*cs != '}');) {
						cs++;
					}
					if(*cs == '}') {
						comment_char = get_named_character(cs - s, s, '\0');
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "DELIMITER=", 10) == 0) {
					s = (char *)skipspace(cs+12);
					for(cs = s; isgraph(*cs) && (*cs != '}');) {
						cs++;
					}
					if(*cs == '}') {
						delimiter_char = get_named_character(cs - s, s, '\0');
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "LABEL=", 6) == 0) {
					s = (char *)skipspace(cs+8);
					for(cs = s; isgraph(*cs) && (*cs != '}');) {
						cs++;
					}
					if(*cs == '}') {
						label_char = get_named_character(cs - s, s, label_char);
						cs++;
						continue;
					}
				} else if(strncasecmp(cs+2, "NOCASE", 6) == 0) {
					cs = (char *)skipspace(cs+8);
					if(*cs == '}') {
						equal = nocase_equal;
						cs++;
						continue;
					}
				}
			} else {
				cs = process_directive(cs+1);
				cs = skipspace(cs);
				if(*cs == '}') {
					cs++;
					continue;
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
		if(!cs) {
			break;
		}
	}

	return failed;
}

static void apply_patches(void) {
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

static bool write_binary_file(void) {
	bool failed = false;

	DEFER(FILE *out = fopen(outfile, "wb"),
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

	return failed;
}

static bool write_header_file(char const *prefix) {
	bool failed = false;

	DEFER(FILE *out = fopen(setfile, "w"),
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
					if(!(sp->tag & HIDDEN_SYMBOL)) {
						if(sp->tag & POSTFIX_SYMBOL) {
							fprintf(out, "#define %.*s_%.*s %llu\n",
								(int)sp->sym.len, (char const *)sp->sym.str,
								(int)set->sym.len, (char const *)set->sym.str,
								sp->val.u
							);
						} else {
							fprintf(out, "#define %.*s_%.*s %llu\n",
								(int)set->sym.len, (char const *)set->sym.str,
								(int)sp->sym.len, (char const *)sp->sym.str,
								sp->val.u
							);
						}
					}
				}
				fprintf(out, "\nstatic char const *%.*s_name[] = {\n",
					(int)set->sym.len, (char const *)set->sym.str
				);
				for(spp = (SYMBOL **)valp->p, end = spp + n; spp < end; ) {
					sp = *spp++;
					if(!(sp->tag & HIDDEN_SYMBOL)) {
						fprintf(out, "\t[%llu] = \"%.*s\",\n",
							sp->val.u, (int)sp->sym.len, (char const *)sp->sym.str
						);
					}
				}
			}
			fprintf(out, "};\n");
		}
		if(prefix) {
			fprintf(out, "\n");
			fprintf(out, "#define %sBYTEBITS %i\n" , prefix, byte_bits);
			fprintf(out, "#define %sSEGMENTS %u\n" , prefix, segments ? 1 : 0);
			fprintf(out, "#define %sMEMORY   %zu\n", prefix, sizeof_memory);
			fprintf(out, "\n");
			for(size_t i = 0; i < N_SYMBOLS; i++) {
				if(symtab[i].ref == 0) {
					continue;
				}
				SYMBOL *sp = &symtab[i].sym;
				if((sp->type != ':') && (sp->type != '=')) {
					continue;
				}
				if(!(sp->tag & HIDDEN_SYMBOL)) {
					fprintf(out, "#define %s%.*s %llu\n",
						prefix,
						(int)sp->sym.len, (char const *)sp->sym.str,
						sp->val.u
					);
				}
			}
		}
		fprintf(out, "\n#endif//ndef %.*s__included\n", setnamelen, setname);
	}

	return failed;
}

static bool write_symbol_file(void) {
	bool failed = false;

	DEFER(FILE *out = fopen(symfile, "wb"),
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
			if(!(sp->tag & HIDDEN_SYMBOL)) {
				names_len += sp->sym.len + 1;
				n_symbols++;
			}
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
			if(!(sp->tag & HIDDEN_SYMBOL)) {
				spp[j++] = sp;
			}
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

	return failed;
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
	{  8, "-e, --error FILE",        "output errors to FILE" },
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
			case 8:
				failed = failed || !(errout = openout(argv[argi], errout));
				break;
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

	failed = process_files(argi, argc, argv);

	if(!failed && (n_xref > 0)) {
		listlen = 0;
		listing("\n; PATCHES\n; =======\n\n");
		apply_patches();
	}
	if(!failed) {
		listlen = 0;
		listing("\n; SIZE = %zu\n\n", maxaddr + 1);
	}
	if(listout && (listout != stdout)) {
		fclose(listout);
	}

	if(!failed && *outfile) {
		failed = write_binary_file();
	}

	if(!failed && *setfile) {
		failed = write_header_file(prefix);
	}

	if(!failed && *symfile) {
		failed = write_symbol_file();
	}

	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
