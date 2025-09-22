#ifndef EVAL_H
#define EVAL_H
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

#define N_EVAL_VARIANTS  (100)

typedef struct eval {
	void     (*trace)(char const *, ...);
	void     (*print)(char const *, ...);
	uint64_t (*emit)(struct eval *e, uint64_t a, uint64_t v);
	uint64_t (*load)(struct eval *e, uint64_t a);
	uint64_t (*func)(struct eval *e, uint16_t a);
	uint64_t*(*symp)(uint64_t a);
	uint64_t (*stag)(uint64_t a, uint64_t);
	uint64_t (*tag) (bool, uint64_t);
	VALUE      v[N_EVAL_VARIANTS];
}
	EVAL;

static void eval_nul_trace(char const *, ...) {
	return;
}

static void eval_nul_print(char const *, ...) {
	return;
}

static uint64_t eval_nul_emit(EVAL *, uint64_t, uint64_t v) {
	return v;
}

static uint64_t eval_nul_load(EVAL *, uint64_t) {
	return 0;
}

static uint64_t eval_nul_func(EVAL *, uint8_t, uint64_t v) {
	return v;
}

static uint64_t *eval_nul_symp(uint64_t) {
	static uint64_t dummy = 0;
	return &dummy;
}

static uint64_t eval_nul_stag(uint64_t, uint64_t) {
	return 0;
}

static uint64_t eval_nul_tag(bool, uint64_t) {
	return 0;
}

#define ENUM_EVAL(ENUM) \
	ENUM(END, "\n") \
\
	ENUM(SEQUENCE, ",") \
\
	ENUM(PARENTHESIS_LEFT, "(") \
	ENUM(PARENTHESIS_RIGHT, ")") \
\
	ENUM(ASSIGN, "=") \
\
	ENUM(CONDITION_IF, "?") \
	ENUM(CONDITION_ELSE, "!") \
\
	ENUM(BOOLEAN_IS, "?") \
	ENUM(BOOLEAN_NOT, "!") \
	ENUM(BOOLEAN_OR, "||") \
	ENUM(BOOLEAN_AND, "&&") \
\
	ENUM(RELATION_EQUAL, "==") \
	ENUM(RELATION_NOT_EQUAL, "!=") \
	ENUM(RELATION_LESS_THAN, "<") \
	ENUM(RELATION_LESS_THAN_OR_EQUAL, "<=") \
	ENUM(RELATION_MORE_THAN, ">") \
	ENUM(RELATION_MORE_THAN_OR_EQUAL, ">=") \
\
	ENUM(BINARY_COMPLIMENT, "~") \
	ENUM(BINARY_MSB, "|") \
	ENUM(BINARY_OR, "|") \
	ENUM(BINARY_AND, "&") \
	ENUM(BINARY_XOR, "^") \
	ENUM(BINARY_LEFT_SHIFT, "<<") \
	ENUM(BINARY_RIGHT_SHIFT, ">>") \
	ENUM(BINARY_LEFT_ROTATE, "<<>") \
	ENUM(BINARY_RIGHT_ROTATE, "<>>") \
\
	ENUM(ARITHMETIC_NEGATE, "-") \
	ENUM(ARITHMETIC_PLUS, "+") \
	ENUM(ARITHMETIC_MINUS, "-") \
	ENUM(ARITHMETIC_MULTIPLY, "*") \
	ENUM(ARITHMETIC_DIVIDE, "/") \
	ENUM(ARITHMETIC_MODULO, "%") \
\
	ENUM(DECREMENT, "--") \
	ENUM(INCREMENT, "++") \
\
	ENUM(EMIT, "@=") \
	ENUM(LOAD, "=@") \
\
	ENUM(TAG_AND, "$&") \
	ENUM(TAG_SET, "$=") \
\
	ENUM(ERROR_CHARACTER,   "invalid character in expression") \
	ENUM(ERROR_OPERATOR,    "unexpected operator in expression") \
	ENUM(ERROR_OPERAND,     "unexpected operand in expression") \
	ENUM(ERROR_PARENTHESIS, "expression parenthesis mismatch") \
	ENUM(ERROR_FUNCTION,    "invalid function name") \
	ENUM(ERROR_INVALID,     "invalid operand") \
\
	ENUM(VARIANT, "$") \
	ENUM(VARIANT_TYPE, "$?") \
\
	ENUM(INDIRECT, "@") \
	ENUM(INDIRECT_TAG_AND, "@&") \
\
	ENUM(FUNCTION, "()") \
\
	ENUM(CONSTANT8, "") \
	ENUM(CONSTANT16, "") \
	ENUM(CONSTANT32, "") \
	ENUM(CONSTANT64, "") \
\
	ENUM(CONSTANT8_COMPLEMENT, "") \
	ENUM(CONSTANT16_COMPLEMENT, "") \
	ENUM(CONSTANT32_COMPLEMENT, "") \
	ENUM(CONSTANT64_COMPLEMENT, "")

#define ENUMERATE(ENUM__name,...)  EVAL_##ENUM__name,
#define TOKENS(ENUM__name,EVAL__token,...)  EVAL__token,

enum {
	ENUM_EVAL(ENUMERATE)
	EVAL_BIT_MASK = 0x40,
	EVAL_MASK_BIT = 0x80,
	EVAL_SMALLINT = 0xC0,
};

static const char *eval_tokens[] = {
	ENUM_EVAL(TOKENS)
};

//------------------------------------------------------------------------------

#define EVAL__IMMEDIATE_SIZE(EVAL__IMMEDIATE_SIZE_limit) \
	(1 \
	+ ((EVAL__IMMEDIATE_SIZE_limit) > (1ull <<  8)) \
	+ ((EVAL__IMMEDIATE_SIZE_limit) > (1ull << 16)) \
	+ ((EVAL__IMMEDIATE_SIZE_limit) > (1ull << 24)) \
	)

static inline void eval__save_immediate(uint64_t a, uint8_t b[], int n) {
	for(int i = 0; i < n; i++) {
		b[i] = (uint8_t)(a >> (i * 8));
	}
	return;
}

static inline uint64_t eval__load_immediate(uint8_t const b[], int n) {
	uint64_t a = 0;
	for(int i = 0; i < n; i++) {
		a |= (uint64_t)b[i] << (i * 8);
	}
	return a;
}

//------------------------------------------------------------------------------

static const char *eval_error_message(int e) {
	if((e >= EVAL_ERROR_CHARACTER) || (e <= EVAL_ERROR_INVALID)) {
		return eval_tokens[e];
	}
	return "invalid expression";
}

#ifdef EVAL_TRACE
static uint8_t const *eval_tokenprint(uint8_t const *cs, EVAL *e) {
	uint64_t l, x = 0;
	uint16_t    a;
	uint8_t     o = *cs++;
	if(o < (sizeof(eval_tokens)/sizeof(*eval_tokens))) {
		e->trace("%s", eval_tokens[o]);
		switch(o) {
		case EVAL_END:
			return NULL;
		case EVAL_IF:
		case EVAL_ELSE:
			a = eval__load_immediate(cs, EVAL__IMMEDIATE_SIZE(N_TOKENS));
			cs += EVAL__IMMEDIATE_SIZE(N_TOKENS);
			e->trace("{%u}", (unsigned)a);
			return cs;
		case EVAL_VARIANT:
			o = *cs++ % N_EVAL_VARIANTS;
			e->trace("%u{%c:0x%"PRIx64"}", (unsigned)o, (int)e->v[o].type, e->v[o].u);
			return cs;
		case EVAL_INDIRECT:
			o = *cs++ % N_EVAL_VARIANTS;
			e->trace("%u{%c:0x%"PRIx64"}", (unsigned)o, (int)e->v[o].type, *e->symp(e->v[o].u));
			return cs;
		case EVAL_FUNCTION:
			a = eval__load_immediate(cs, EVAL__IMMEDIATE_SIZE(N_FUNCTIONS));
			cs += EVAL__IMMEDIATE_SIZE(N_FUNCTIONS);
			e->trace("{%u}", (unsigned)a);
			return cs;
		case EVAL_CONSTANT8_COMPLEMENT:
			x = ~x; // fallthrough
		case EVAL_CONSTANT8:
			l  = *cs++;
			l ^= x;
			e->trace("0x%"PRIx64, l);
			return cs;
		case EVAL_CONSTANT16_COMPLEMENT:
			x = ~x; // fallthrough
		case EVAL_CONSTANT16:
			l  = *cs++;
			l |= (uint64_t)*cs++ << 8;
			l ^= x;
			e->trace("0x%"PRIx64, l);
			return cs;
		case EVAL_CONSTANT32_COMPLEMENT:
			x = ~x; // fallthrough
		case EVAL_CONSTANT32:
			l  = *cs++;
			l |= (uint64_t)*cs++ << 8;
			l |= (uint64_t)*cs++ << 16;
			l |= (uint64_t)*cs++ << 24;
			l ^= x;
			e->trace("0x%"PRIx64, l);
			return cs;
		case EVAL_CONSTANT64_COMPLEMENT:
			x = ~x; // fallthrough
		case EVAL_CONSTANT64:
			l  = *cs++;
			l |= (uint64_t)*cs++ << 8;
			l |= (uint64_t)*cs++ << 16;
			l |= (uint64_t)*cs++ << 24;
			l |= (uint64_t)*cs++ << 32;
			l |= (uint64_t)*cs++ << 40;
			l |= (uint64_t)*cs++ << 48;
			l |= (uint64_t)*cs++ << 56;
			l ^= x;
			e->trace("0x%"PRIx64, l);
			return cs;
		default:
			return cs;
		}
	}
	switch(o & EVAL_SMALLINT) {
	case EVAL_BIT_MASK:
		l = ~UINT64_C(0) >> (o & 63u);
		e->trace("%.16"PRIx64, l);
		return cs;
	case EVAL_MASK_BIT:
		l = UINT64_C(1) << (o & 63u);
		e->trace("%.16"PRIx64, l);
		return cs;
	case EVAL_SMALLINT:
		l = o & 63u;
		e->trace("%"PRIu64, l);
		return cs;
	default:
		e->trace("{invalid eval token: %u}", (unsigned)o);
		return NULL;
	}
}

static void eval_detokenize(uint8_t const *cs, EVAL *e) {
	while((cs = eval_tokenprint(cs, e)) != NULL)
		;
}

#	define EVAL_TOKENPRINT(EVAL_TOKENPRINT__cs,EVAL_TOKENPRINT__e) \
		do { \
			if(EVAL_TOKENPRINT__e) eval_tokenprint(EVAL_TOKENPRINT__cs,EVAL_TOKENPRINT__e); \
		} while(0)
#else
#	define EVAL_TOKENPRINT(EVAL_TOKENPRINT__cs,EVAL_TOKENPRINT__e) \
		do { ; } while(0)
#endif//def EVAL_TRACE

//------------------------------------------------------------------------------

static int evalf_getc(void *p) {
	int c = fgetc(p);
	return (c == EOF) ? '\0' : c;
}

static int evalf_ungetc(int c, void *p) {
	return c ? ungetc(c, p) : c;
}

static int evals_getc(void *p) {
	int c = *(*(char const **)p);
	if(c) (*(char const **)p)++;
	return c;
}

static int evals_ungetc(int c, void *p) {
	if(c) (*(char const **)p)--;
	return c;
}

static uintmax_t eval_getuint(int c, void *p, int (*get)(void *), int (*unget)(int, void *)) {
	uintmax_t u = 0;
	if(c == '0') {
		do {
			c = get(p);
		} while(c == '0')
			;
		switch(c) {
		case 'B': case 'b':
			c = get(p);
			while((c == '\'') || (('0' <= c) && (c <= '1'))) {
				if(c != '\'') {
					u = (u * 2) + (c - '0');
				}
				c = get(p);
			}
			break;
		case 'X': case 'x':
			c = get(p);
			for(int upper = 0, lower = 0;
				(c == '\'')
					|| (('0' <= c) && (c <= '9'))
					|| (upper = (('A' <= c) && (c <= 'F')))
					|| (lower = (('a' <= c) && (c <= 'f')));
				upper = 0, lower = 0
			) {
				if(c != '\'') {
					u = (u * 16) + (upper ? (10 + c - 'A') : (lower ? (10 + c - 'a') : (c - '0')));
				}
				c = get(p);
			}
			break;
		default :
			while((c == '\'') || (('0' <= c) && (c <= '7'))) {
				if(c != '\'') {
					u = (u * 8) + (c - '0');
				}
				c = get(p);
			}
			break;
		}
	} else {
		while((c == '\'') || (('0' <= c) && (c <= '9'))) {
			if(c != '\'') {
				u = (u * 10) + (c - '0');
			}
			c = get(p);
		}
	}
	unget(c, p);
	return u;
}

static STRING eval_tokenize(
	void   *p,
	int   (*get)(void *),
	int   (*unget)(int, void *),
	int   (*getfn)(size_t n, char const s[]),
	int   (*getconst)(size_t n, char const s[], uint64_t *p),
	int     end,
	STRING  token,
	size_t *linenop
) {
	STRING t = STRING(0, token.str);
#	define EVAL_TOKENIZE__ERROR(EVAL_TOKENIZE__error) \
	if((t.len == 0) || (((uint8_t *)t.str)[t.len - 1] != (EVAL_TOKENIZE__error))) { \
		((uint8_t *)t.str)[t.len++] = EVAL_TOKENIZE__error; \
	} else do {;} while(0)
	for(int paren = 0, state = 1, c;;) {
		while((c = get(p)) && !isgraph(c)) {
			if(c == '\n') {
				++*linenop;
			}
		}
		if(t.len >= token.len) {
			errno = ENOMEM;
			return STRING(0, "");
		}
		switch(c) {
			uint64_t u;
		case ';':
		case '#':
			while((c = get(p))) {
				if(c == '\n') {
					++*linenop;
					break;
				}
			}
			continue;
		case ',':
			if(state > 1) {
				((uint8_t *)t.str)[t.len++] = EVAL_SEQUENCE;
				state = 1;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '(':
			if(state < 2) {
				paren++;
				((uint8_t *)t.str)[t.len++] = EVAL_PARENTHESIS_LEFT;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case ')':
			if(state > 1) {
				if(paren > 0) {
					paren--;
					((uint8_t *)t.str)[t.len++] = EVAL_PARENTHESIS_RIGHT;
					state = 2;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_PARENTHESIS);
				state = 2;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '=':
			c = get(p);
			switch(c) {
			case '=':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_RELATION_EQUAL;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			case '@':
				if(state < 2) {
					((uint8_t *)t.str)[t.len++] = EVAL_LOAD;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_ASSIGN;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			}
		case '?':
			if(state > 1) {
				if(t.len < (token.len - EVAL__IMMEDIATE_SIZE(N_TOKENS))) {
					((uint8_t *)t.str)[t.len++] = EVAL_CONDITION_IF;
					eval__save_immediate(0, &((uint8_t *)t.str)[t.len], EVAL__IMMEDIATE_SIZE(N_TOKENS));
					t.len += EVAL__IMMEDIATE_SIZE(N_TOKENS);
					state = 1;
					continue;
				}
				errno = ENOMEM;
				return STRING(0, "");
			}
			((uint8_t *)t.str)[t.len++] = EVAL_BOOLEAN_IS;
			continue;
		case '!':
			c = get(p);
			switch(c) {
			case '=':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_RELATION_NOT_EQUAL;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					if(t.len < (token.len - EVAL__IMMEDIATE_SIZE(N_TOKENS))) {
						((uint8_t *)t.str)[t.len++] = EVAL_CONDITION_ELSE;
						eval__save_immediate(0, &((uint8_t *)t.str)[t.len], EVAL__IMMEDIATE_SIZE(N_TOKENS));
						t.len += EVAL__IMMEDIATE_SIZE(N_TOKENS);
						state = 1;
						continue;
					}
					errno = ENOMEM;
					return STRING(0, "");
				}
				((uint8_t *)t.str)[t.len++] = EVAL_BOOLEAN_NOT;
				continue;
			}
		case '&':
			c = get(p);
			switch(c) {
			case '&':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_BOOLEAN_AND;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_BINARY_AND;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			}
		case '|':
			c = get(p);
			switch(c) {
			case '|':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_BOOLEAN_OR;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_BINARY_OR;
					state = 1;
					continue;
				}
				((uint8_t *)t.str)[t.len++] = EVAL_BINARY_MSB;
				continue;
			}
		case '<':
			c = get(p);
			switch(c) {
			case '<':
				c = get(p);
				switch(c) {
				case '>':
					if(state > 1) {
						((uint8_t *)t.str)[t.len++] = EVAL_BINARY_LEFT_ROTATE;
						state = 1;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
					continue;
				default :
					unget(c, p);
					if(state > 1) {
						((uint8_t *)t.str)[t.len++] = EVAL_BINARY_LEFT_SHIFT;
						state = 1;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
					continue;
				}
			case '>':
				c = get(p);
				switch(c) {
				case '>':
					if(state > 1) {
						((uint8_t *)t.str)[t.len++] = EVAL_BINARY_RIGHT_ROTATE;
						state = 1;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
					continue;
				default :
					unget(c, p);
					if(state > 1) {
						((uint8_t *)t.str)[t.len++] = EVAL_RELATION_NOT_EQUAL;
						state = 1;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
					continue;
				}
			case '=':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_RELATION_LESS_THAN_OR_EQUAL;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_RELATION_LESS_THAN;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			}
		case '>':
			c = get(p);
			switch(c) {
			case '>':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_BINARY_RIGHT_SHIFT;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			case '=':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_RELATION_MORE_THAN_OR_EQUAL;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_RELATION_MORE_THAN;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			}
		case '^':
			if(state > 1) {
				((uint8_t *)t.str)[t.len++] = EVAL_BINARY_XOR;
				state = 1;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '~':
			if(state < 2) {
				((uint8_t *)t.str)[t.len++] = EVAL_BINARY_COMPLIMENT;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '+':
			c = get(p);
			switch(c) {
			case '+':
				if(state > 2) {
					((uint8_t *)t.str)[t.len++] = EVAL_INCREMENT;
					state = 2;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				state = 2;
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_PLUS;
					state = 1;
					continue;
				}
//              REDUNDANT: ELIDED
//				((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_PLUS;
				continue;
			}
		case '-':
			c = get(p);
			switch(c) {
			case '-':
				if(state > 2) {
					((uint8_t *)t.str)[t.len++] = EVAL_DECREMENT;
					state = 2;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				state = 2;
				continue;
			default :
				unget(c, p);
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_MINUS;
					state = 1;
					continue;
				}
				((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_NEGATE;
				continue;
			}
		case '*':
			if(state > 1) {
				((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_MULTIPLY;
				state = 1;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '/':
			if(state > 1) {
				((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_DIVIDE;
				state = 1;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '%':
			if(state > 1) {
				((uint8_t *)t.str)[t.len++] = EVAL_ARITHMETIC_MODULO;
				state = 1;
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
			continue;
		case '$':
			c = get(p);
			if(isdigit(c)) {
				if(t.len >= (token.len - 1)) {
					errno = ENOMEM;
					return STRING(0, "");
				}
				c = eval_getuint(c, p, get, unget) % N_EVAL_VARIANTS;
				((uint8_t *)t.str)[t.len++] = EVAL_VARIANT;
				((uint8_t *)t.str)[t.len++] = c;
				if(state < 2) {
					state = 3;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
				continue;
			}
			switch(c) {
			case '?':
				c = get(p);
				if(isdigit(c)) {
					if(t.len >= (token.len - 1)) {
						errno = ENOMEM;
						return STRING(0, "");
					}
					c = eval_getuint(c, p, get, unget) % N_EVAL_VARIANTS;
					((uint8_t *)t.str)[t.len++] = EVAL_VARIANT_TYPE;
					((uint8_t *)t.str)[t.len++] = c;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				break;
			case '&':
				if(state < 2) {
					((uint8_t *)t.str)[t.len++] = EVAL_TAG_AND;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
				continue;
			case '=':
				if(state < 2) {
					((uint8_t *)t.str)[t.len++] = EVAL_TAG_SET;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_CHARACTER);
			state = 1;
			continue;
		case '@':
			c = get(p);
			if(isdigit(c)) {
				if(t.len >= (token.len - 1)) {
					errno = ENOMEM;
					return STRING(0, "");
				}
				c = eval_getuint(c, p, get, unget) % N_EVAL_VARIANTS;
				((uint8_t *)t.str)[t.len++] = EVAL_INDIRECT;
				((uint8_t *)t.str)[t.len++] = c;
				if(state < 2) {
					state = 2;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
				continue;
			}
			switch(c) {
			case '&':
				c = get(p);
				if(isdigit(c)) {
					if(t.len >= (token.len - 1)) {
						errno = ENOMEM;
						return STRING(0, "");
					}
					c = eval_getuint(c, p, get, unget) % N_EVAL_VARIANTS;
					((uint8_t *)t.str)[t.len++] = EVAL_INDIRECT_TAG_AND;
					((uint8_t *)t.str)[t.len++] = c;
					if(state < 2) {
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				break;
			case '=':
				if(state > 1) {
					((uint8_t *)t.str)[t.len++] = EVAL_EMIT;
					state = 1;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERATOR);
				continue;
			}
			EVAL_TOKENIZE__ERROR(EVAL_ERROR_CHARACTER);
			state = 1;
			continue;
		case '\'':
			c = get(p);
			if(c == '\\') {
				c = get(p);
				switch(c) {
				default : break;
				case 'a': c = '\a'; break;
				case 'b': c = '\b'; break;
				case 'f': c = '\f'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				case 'v': c = '\v'; break;
				}
			}
			u = c & 255;
			c = get(p);
			if(c != '\'') {
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_CHARACTER);
				if(state < 2) {
					state = 2;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
				continue;
			}
			goto case_uint;
		default:
			if(isdigit(c)) {
				u = eval_getuint(c, p, get, unget);
		case_uint:
				if(u <= 63u) {
					((uint8_t *)t.str)[t.len++] = EVAL_SMALLINT | u;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if((u & (u - 1)) == 0) {
					unsigned b = 0;
					for(uint64_t m = 1; m != u; m <<= 1) {
						b++;
					}
					((uint8_t *)t.str)[t.len++] = EVAL_MASK_BIT | b;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if((u & (u + 1)) == 0) {
					unsigned b = 0;
					for(uint64_t m = ~UINT64_C(0); m != u; m >>= 1) {
						b++;
					}
					((uint8_t *)t.str)[t.len++] = EVAL_BIT_MASK | b;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				int      q = (~u < u);
				uint64_t x = q ? ~u : u;
				if(x <= UINT8_MAX) {
					if(t.len >= token.len) {
						errno = ENOMEM;
						return STRING(0, "");
					}
					((uint8_t *)t.str)[t.len++] = q ? EVAL_CONSTANT8_COMPLEMENT : EVAL_CONSTANT8;
					((uint8_t *)t.str)[t.len++] = x & 255u;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if(x <= UINT16_MAX) {
					if(t.len >= (token.len - 2)) {
						errno = ENOMEM;
						return STRING(0, "");
					}
					((uint8_t *)t.str)[t.len++] = q ? EVAL_CONSTANT16_COMPLEMENT : EVAL_CONSTANT16;
					((uint8_t *)t.str)[t.len++] =  x       & 255u;
					((uint8_t *)t.str)[t.len++] = (x >> 8) & 255u;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if(x <= UINT32_MAX) {
					if(t.len >= (token.len - 4)) {
						errno = ENOMEM;
						return STRING(0, "");
					}
					((uint8_t *)t.str)[t.len++] = q ? EVAL_CONSTANT32_COMPLEMENT : EVAL_CONSTANT32;
					((uint8_t *)t.str)[t.len++] =  x        & 255u;
					((uint8_t *)t.str)[t.len++] = (x >>  8) & 255u;
					((uint8_t *)t.str)[t.len++] = (x >> 16) & 255u;
					((uint8_t *)t.str)[t.len++] = (x >> 24) & 255u;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if(t.len >= (token.len - 8)) {
					errno = ENOMEM;
					return STRING(0, "");
				}
				((uint8_t *)t.str)[t.len++] = q ? EVAL_CONSTANT64_COMPLEMENT : EVAL_CONSTANT64;
				((uint8_t *)t.str)[t.len++] =  x        & 255u;
				((uint8_t *)t.str)[t.len++] = (x >>  8) & 255u;
				((uint8_t *)t.str)[t.len++] = (x >> 16) & 255u;
				((uint8_t *)t.str)[t.len++] = (x >> 24) & 255u;
				((uint8_t *)t.str)[t.len++] = (x >> 32) & 255u;
				((uint8_t *)t.str)[t.len++] = (x >> 40) & 255u;
				((uint8_t *)t.str)[t.len++] = (x >> 48) & 255u;
				((uint8_t *)t.str)[t.len++] = (x >> 56) & 255u;
				if(state < 2) {
					state = 2;
					continue;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
				continue;
			}
			if(isalpha(c) || (c == '_') || (c == '`')) {
				size_t n = 1;
				char   fn[65] = { c };
				bool   noerr = true;
				c = get(p);
				for(bool quote = false;
					(quote && isgraph(c)) || isalnum(c) || (c == '_') || (c == '`');
					c = get(p)
				) {
					if(n > 63) {
						if(noerr) {
							EVAL_TOKENIZE__ERROR(EVAL_ERROR_FUNCTION);
							noerr = false;
						}
						continue;
					}
					fn[n++] = c;
					quote = !quote && (c == '`');
				}
				fn[n] = '\0';
				unget(c, p);
				if((strcmp(fn, "TRUE") == 0) || (strcmp(fn, "true") == 0)) {
					((uint8_t *)t.str)[t.len++] = EVAL_SMALLINT | 1;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if((strcmp(fn, "FALSE") == 0) || (strcmp(fn, "false") == 0)) {
					((uint8_t *)t.str)[t.len++] = EVAL_SMALLINT | 0;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				if((strcmp(fn, "INVALID") == 0) || (strcmp(fn, "invalid") == 0)) {
					((uint8_t *)t.str)[t.len++] = EVAL_ERROR_INVALID;
					if(state < 2) {
						state = 2;
						continue;
					}
					EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
					continue;
				}
				int i = getfn(n, fn);
				if(i >= 0) {
					if(t.len < (token.len - EVAL__IMMEDIATE_SIZE(N_FUNCTIONS))) {
						((uint8_t *)t.str)[t.len++] = EVAL_FUNCTION;
						eval__save_immediate(i, &((uint8_t *)t.str)[t.len], EVAL__IMMEDIATE_SIZE(N_FUNCTIONS));
						t.len += EVAL__IMMEDIATE_SIZE(N_FUNCTIONS);
						if(state < 2) {
							state = 2;
							continue;
						}
						EVAL_TOKENIZE__ERROR(EVAL_ERROR_OPERAND);
						continue;
					}
					errno = ENOMEM;
					return STRING(0, "");
				}
				i = getconst(n, fn, &u);
				if(i >= 0) {
					goto case_uint;
				}
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_FUNCTION);
				state = 2;
				continue;
			}
			if(c != end) {
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_CHARACTER);
				state = 2;
				continue;
			}
			// fallthrough
		case '\0':
			if(paren != 0) {
				EVAL_TOKENIZE__ERROR(EVAL_ERROR_PARENTHESIS);
			}
			((uint8_t *)t.str)[t.len++] = EVAL_END;
			return t;
		}
	}
#	undef EVAL_TOKENIZE__ERROR
}

//------------------------------------------------------------------------------

static uint64_t eval_sequence(uint8_t const *cs, EVAL *e, uint8_t const **csp);

static uint64_t eval_primary(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = 0, x = 0;
	uint16_t a;
	uint8_t  o = *cs;
	switch(o) {
	case EVAL_PARENTHESIS_LEFT:
		EVAL_TOKENPRINT(cs, e);
		l = eval_sequence(cs + 1, e, csp);
		if(*(cs = *csp) == EVAL_PARENTHESIS_RIGHT) {
			EVAL_TOKENPRINT(cs, e);
			*csp = cs + 1;
		}
		return l;
	case EVAL_VARIANT:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		if(*(cs + 1) == EVAL_INCREMENT) {
			EVAL_TOKENPRINT(cs + 1, e);
			l = e ? e->v[*cs % N_EVAL_VARIANTS].u++ : 0;
			*csp = cs + 2;
			return l;
		}
		if(*(cs + 1) == EVAL_DECREMENT) {
			EVAL_TOKENPRINT(cs + 1, e);
			l = e ? e->v[*cs % N_EVAL_VARIANTS].u-- : 0;
			*csp = cs + 2;
			return l;
		}
		l = e ? e->v[*cs].u : 0;
		*csp = cs + 1;
		return l;
	case EVAL_INDIRECT:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		if(*(cs + 1) == EVAL_INCREMENT) {
			EVAL_TOKENPRINT(cs + 1, e);
			l = e ? (*e->symp(e->v[*cs % N_EVAL_VARIANTS].u))++ : 0;
			*csp = cs + 2;
			return l;
		}
		if(*(cs + 1) == EVAL_DECREMENT) {
			EVAL_TOKENPRINT(cs + 1, e);
			l = e ? (*e->symp(e->v[*cs % N_EVAL_VARIANTS].u))-- : 0;
			*csp = cs + 2;
			return l;
		}
		l = e ? *e->symp(e->v[*cs % N_EVAL_VARIANTS].u) : 0;
		*csp = cs + 1;
		return l;
	case EVAL_FUNCTION:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		a = eval__load_immediate(cs, EVAL__IMMEDIATE_SIZE(N_FUNCTIONS));
		cs += EVAL__IMMEDIATE_SIZE(N_FUNCTIONS);
		l = e ? e->func(e, a) : 0;
		*csp = cs;
		return l;
	case EVAL_CONSTANT8_COMPLEMENT:
		x = ~x; // fallthrough
	case EVAL_CONSTANT8:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		l  = *cs++;
		l ^= x;
		*csp = cs;
		return l;
	case EVAL_CONSTANT16_COMPLEMENT:
		x = ~x; // fallthrough
	case EVAL_CONSTANT16:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		l  = *cs++;
		l |= (uint64_t)*cs++ << 8;
		l ^= x;
		*csp = cs;
		return l;
	case EVAL_CONSTANT32_COMPLEMENT:
		x = ~x; // fallthrough
	case EVAL_CONSTANT32:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		l  = *cs++;
		l |= (uint64_t)*cs++ << 8;
		l |= (uint64_t)*cs++ << 16;
		l |= (uint64_t)*cs++ << 24;
		l ^= x;
		*csp = cs;
		return l;
	case EVAL_CONSTANT64_COMPLEMENT:
		x = ~x; // fallthrough
	case EVAL_CONSTANT64:
		EVAL_TOKENPRINT(cs, e);
		cs++;
		l  = *cs++;
		l |= (uint64_t)*cs++ << 8;
		l |= (uint64_t)*cs++ << 16;
		l |= (uint64_t)*cs++ << 24;
		l |= (uint64_t)*cs++ << 32;
		l |= (uint64_t)*cs++ << 40;
		l |= (uint64_t)*cs++ << 48;
		l |= (uint64_t)*cs++ << 56;
		l ^= x;
		*csp = cs;
		return l;
	case EVAL_ERROR_INVALID:
		if(e) {
			e->print(eval_error_message(o));
		}
		*csp = cs + 1;
		return l;
	default:
		switch(o & EVAL_SMALLINT) {
		case EVAL_BIT_MASK:
			EVAL_TOKENPRINT(cs, e);
			l = ~UINT64_C(0) >> (o & 63u);
			*csp = cs + 1;
			return l;
		case EVAL_MASK_BIT:
			EVAL_TOKENPRINT(cs, e);
			l = UINT64_C(1) << (o & 63u);
			*csp = cs + 1;
			return l;
		case EVAL_SMALLINT:
			EVAL_TOKENPRINT(cs, e);
			l = o & 63u;
			*csp = cs + 1;
			return l;
		default:
			*csp = cs;
			return l;
		}
	}
}

static uint64_t eval_assignment(uint8_t const *cs, EVAL *e, uint8_t const **csp);

static uint64_t eval_unary(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = 0;
	int o = *cs;
	switch(o) {
	case EVAL_BOOLEAN_IS:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		return !!l;
	case EVAL_BOOLEAN_NOT:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		return !l;
	case EVAL_BINARY_MSB:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		l = msbit(l);
		return l;
	case EVAL_BINARY_COMPLIMENT:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		return ~l;
	case EVAL_ARITHMETIC_NEGATE:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		return -l;
//	case EVAL_ARITHMETIC_PLUS:
//		EVAL_TOKENPRINT(cs, e);
//		l = eval_unary(cs + 1, e, csp);
//		return l;
	case EVAL_TAG_AND:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		l = e ? e->tag(false, l) : 0;
		return l;
	case EVAL_TAG_SET:
		EVAL_TOKENPRINT(cs, e);
		l = eval_assignment(cs + 1, e, csp);
		l = e ? e->tag(true, l) : 0;
		return l;
	case EVAL_INDIRECT_TAG_AND:
		EVAL_TOKENPRINT(cs, e);
		l = eval_unary(cs + 1, e, csp);
		l = e ? e->stag(e->v[*cs % N_EVAL_VARIANTS].u, l) : 0;
		return l;
	case EVAL_VARIANT_TYPE:
		EVAL_TOKENPRINT(cs, e);
		l = e ? e->v[*(cs + 1) % N_EVAL_VARIANTS].type : 0;
		*csp = cs + 2;
		return l;
	case EVAL_LOAD:
		EVAL_TOKENPRINT(cs, e);
		l = eval_assignment(cs + 1, e, csp);
		l = e ? e->load(e, l) : 0;
		return l;
	default:
		l = eval_primary(cs, e, csp);
		return l;
	}
}

static uint64_t eval_exponentive(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_unary(cs, e, csp);
	for(uint8_t o; (o = *(cs = *csp)) != EVAL_END;) {
		switch(o) {
			uint64_t r;
		case EVAL_BINARY_LEFT_SHIFT:
			EVAL_TOKENPRINT(cs, e);
			r = eval_unary(cs + 1, e, csp);
			l = l << (r & 63u);
			continue;
		case EVAL_BINARY_RIGHT_SHIFT:
			EVAL_TOKENPRINT(cs, e);
			r = eval_unary(cs + 1, e, csp);
			l = l >> (r & 63u);
			continue;
		case EVAL_BINARY_LEFT_ROTATE:
			EVAL_TOKENPRINT(cs, e);
			r = eval_unary(cs + 1, e, csp);
			l = (l << (r & 63u)) | (l >> (-r & 63u));
			continue;
		case EVAL_BINARY_RIGHT_ROTATE:
			EVAL_TOKENPRINT(cs, e);
			r = eval_unary(cs + 1, e, csp);
			l = (l << (-r & 63u)) | (l >> (r & 63u));
			continue;
		default:
			return l;
		}
	}
	return l;
}

static uint64_t eval_multiplicative(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_exponentive(cs, e, csp);
	for(uint8_t o; (o = *(cs = *csp)) != EVAL_END;) {
		switch(o) {
			uint64_t r;
		case EVAL_ARITHMETIC_MULTIPLY:
			EVAL_TOKENPRINT(cs, e);
			r = eval_exponentive(cs + 1, e, csp);
			l = l * r;
			continue;
		case EVAL_ARITHMETIC_DIVIDE:
			EVAL_TOKENPRINT(cs, e);
			r = eval_exponentive(cs + 1, e, csp);
			l = r ? l / r : 0;
			continue;
		case EVAL_ARITHMETIC_MODULO:
			EVAL_TOKENPRINT(cs, e);
			r = eval_exponentive(cs + 1, e, csp);
			l = r ? l % r : 0;
			continue;
		default:
			return l;
		}
	}
	return l;
}

static uint64_t eval_additive(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_multiplicative(cs, e, csp);
	for(uint8_t o; (o = *(cs = *csp)) != EVAL_END;) {
		switch(o) {
			uint64_t r;
		case EVAL_ARITHMETIC_PLUS:
			EVAL_TOKENPRINT(cs, e);
			r = eval_multiplicative(cs + 1, e, csp);
			l = l + r;
			continue;
		case EVAL_ARITHMETIC_MINUS:
			EVAL_TOKENPRINT(cs, e);
			r = eval_multiplicative(cs + 1, e, csp);
			l = l - r;
			continue;
		default:
			return l;
		}
	}
	return l;
}

static uint64_t eval_bitwise(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_additive(cs, e, csp);
	for(uint8_t o; (o = *(cs = *csp)) != EVAL_END;) {
		switch(o) {
			uint64_t r;
		case EVAL_BINARY_OR:
			EVAL_TOKENPRINT(cs, e);
			r = eval_additive(cs + 1, e, csp);
			l = l | r;
			continue;
		case EVAL_BINARY_AND:
			EVAL_TOKENPRINT(cs, e);
			r = eval_additive(cs + 1, e, csp);
			l = l & r;
			continue;
		case EVAL_BINARY_XOR:
			EVAL_TOKENPRINT(cs, e);
			r = eval_additive(cs + 1, e, csp);
			l = l ^ r;
			continue;
		default:
			return l;
		}
	}
	return l;
}

static uint64_t eval_relation(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_bitwise(cs, e, csp);
	for(uint8_t o; (o = *(cs = *csp)) != EVAL_END;) {
		switch(o) {
			uint64_t r;
		case EVAL_RELATION_EQUAL:
			EVAL_TOKENPRINT(cs, e);
			r = eval_bitwise(cs + 1, e, csp);
			l = l == r;
			continue;
		case EVAL_RELATION_NOT_EQUAL:
			EVAL_TOKENPRINT(cs, e);
			r = eval_bitwise(cs + 1, e, csp);
			l = l != r;
			continue;
		case EVAL_RELATION_LESS_THAN:
			EVAL_TOKENPRINT(cs, e);
			r = eval_bitwise(cs + 1, e, csp);
			l = l < r;
			continue;
		case EVAL_RELATION_LESS_THAN_OR_EQUAL:
			EVAL_TOKENPRINT(cs, e);
			r = eval_bitwise(cs + 1, e, csp);
			l = l <= r;
			continue;
		case EVAL_RELATION_MORE_THAN:
			EVAL_TOKENPRINT(cs, e);
			r = eval_bitwise(cs + 1, e, csp);
			l = l > r;
			continue;
		case EVAL_RELATION_MORE_THAN_OR_EQUAL:
			EVAL_TOKENPRINT(cs, e);
			r = eval_bitwise(cs + 1, e, csp);
			l = l >= r;
			continue;
		default:
			return l;
		}
	}
	return l;
}

static uint64_t eval_boolean(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_relation(cs, e, csp);
	for(uint8_t o; (o = *(cs = *csp)) != EVAL_END;) {
		switch(o) {
			uint64_t r;
		case EVAL_BOOLEAN_OR:
			EVAL_TOKENPRINT(cs, e);
			r = eval_relation(cs + 1, e, csp);
			l = l || r;
			continue;
		case EVAL_BOOLEAN_AND:
			EVAL_TOKENPRINT(cs, e);
			r = eval_relation(cs + 1, e, csp);
			l = l && r;
			continue;
		default:
			return l;
		}
	}
	return l;
}

static uint64_t eval_condition(uint8_t const *cs, EVAL *e, uint8_t const **csp);

static uint64_t eval_alternation(uint8_t const *cs, EVAL *e, uint8_t const **csp, uint64_t cond) {
	uint8_t const *ct = cs;
	uint64_t a = eval__load_immediate(cs, EVAL__IMMEDIATE_SIZE(N_TOKENS));
	cs += EVAL__IMMEDIATE_SIZE(N_TOKENS);
	uint64_t l = 0;
	if(a && !cond) {
		*csp = cs += a;
	} else {
		l = eval_condition(cs, cond ? e : NULL, csp);
		cs = *csp;
		if(!a) {
			a = cs - ct - EVAL__IMMEDIATE_SIZE(N_TOKENS);
			eval__save_immediate(a, (uint8_t *)ct, EVAL__IMMEDIATE_SIZE(N_TOKENS));
		}
	}
	if(*cs == EVAL_CONDITION_ELSE) {
		EVAL_TOKENPRINT(cs, e);
		cs++;
		ct = cs;
		a = eval__load_immediate(cs, EVAL__IMMEDIATE_SIZE(N_TOKENS));
		cs += EVAL__IMMEDIATE_SIZE(N_TOKENS);
		if(a && cond) {
			*csp = cs += a;
		} else {
			uint64_t r = eval_condition(cs, !cond ? e : NULL, csp);
			cs = *csp;
			if(!a) {
				a = cs - ct - EVAL__IMMEDIATE_SIZE(N_TOKENS);
				eval__save_immediate(a, (uint8_t *)ct, EVAL__IMMEDIATE_SIZE(N_TOKENS));
			}
			if(!cond) {
				return r;
			}
		}
	}
	return l;
}

static uint64_t eval_condition(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_boolean(cs, e, csp);
	while(*(cs = *csp) == EVAL_CONDITION_IF) {
		EVAL_TOKENPRINT(cs, e);
		l = eval_alternation(cs + 1, e, csp, l);
	}
	return l;
}

static uint64_t eval_assignment(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l;
	uint8_t  o = *cs;
	switch(o) {
	case EVAL_VARIANT:
	case EVAL_INDIRECT:
		if(*(cs + 2) == EVAL_ASSIGN) {
			EVAL_TOKENPRINT(cs, e);
			EVAL_TOKENPRINT(cs + 2, e);
			int i = *(cs + 1) % N_EVAL_VARIANTS;
			l = eval_assignment(cs + 3, e, csp);
			if(e) {
				if(o == EVAL_INDIRECT) {
					*e->symp(e->v[i].u) = l;
				} else {
					e->v[i].u = l;
				}
			}
			return l;
		}
		// fallthrough
	default:
		l = eval_condition(cs, e, csp);
		if(*(cs = *csp) == EVAL_EMIT) {
			EVAL_TOKENPRINT(cs, e);
			uint64_t r = eval_assignment(cs + 1, e, csp);
			l = e ? e->emit(e, l, r) : r;
		}
		return l;
	}
}

static uint64_t eval_sequence(uint8_t const *cs, EVAL *e, uint8_t const **csp) {
	uint64_t l = eval_assignment(cs, e, csp);
	while(*(cs = *csp) == EVAL_SEQUENCE) {
		EVAL_TOKENPRINT(cs, e);
		l = eval_assignment(cs + 1, e, csp);
	}
	return l;
}

//------------------------------------------------------------------------------

static int eval_verify_expression(uint8_t const *cs, uint8_t const **csp) {
	if(!csp) csp = &cs;
	(void)eval_sequence(cs, NULL, csp);
	return *cs;
}
#define eval_verify_expression(eval_verify_expression__cs,...) \
	(eval_verify_expression)((eval_verify_expression__cs),(__VA_ARGS__+0))

static inline uint64_t eval_expression(uint8_t const *cs, EVAL *e) {
	uint64_t l = eval_sequence(cs, e, &cs);
	EVAL_TOKENPRINT(cs, e);
	return l;
}

//------------------------------------------------------------------------------

#endif//ndef EVAL_H
