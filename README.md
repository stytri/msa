# msa

## Version 1.3.0

## Command Line

```
Matching String Assembler: a simple assembler for virtual machines.
usage: msa [OPTION]... [FILE]...
options:
  -h, --help             display this help and exit
      --version          display version and exit
      --license          display license and exit
      --readme           display readme and exit
  -o, --output FILE      output to FILE
  -l, --listing FILE     write listing to FILE
  -s, --set-header FILE  write header FILE for sets
  -y, --symbols PFIX     include non-set symbols, adding prefix PFIX, in set header
```

## Description

**msa** provides a basic assembler to test the instruction sets of virtual machines.

**msa** does this by parsing assembler source files and creating a pattern string, which it then looks up in a hash table; it then executes an associated expression to output binary data. For patterns with a target address, another expression can be run as a linker.

The patterns and their expressions are defined in `.msa` files.

**N.B.** in the following, the term **byte** is the minimum addressable unit of memory of the virtual machine.

## .msa files

`.msa` files are recognised by the presence of the substring/extension `.msa` in the file name.

The following characters have special meaning in `.msa` files: `#` `;` `{` `}`

### comments

Comments are introduced by the `;` character, and continue to the end of line.

### line directives

C preprocessor `#line` directives are recognised. Other lines with an initial `#` character are ignored.

### directives

**msa** directives are enclosed by `{` and `}`. There are three types of directives: those that configure the assembler, those that define reserved identifiers; e.g instruction and register names, and those that define functions.

#### configuration directives

These directives are identified by a initial `$`:

`$BYTEBITS=` _bits_
	where _bits_ is one of `8` `16` `32` `64`; default is `8`.

`$BYTEFORMAT=` _format_
	where _format_ is one of `OCTAL` `DECIMAL` `HEXADECIMAL`
	used when printing numbers in listings; default is `HEXADECIMAL`.

`$ADDRESS=` _replacement_
	where _replacement_ is a sequence of graphical characters excluding '}'.
	when an address (i.e. undeclared symbol - see **identifier directives** below) is encountered in the assembly source, its is replaced by _replacement_ in the pattern.
	default is `%a`

`$INTEGER=` _replacement_
	where _replacement_ is a sequence of graphical characters excluding '}'.
	when an integer value is encountered in the assembly source, its is replaced by _replacement_ in the pattern.
	default is `%i`

`$MEMORY=` _size_
	where _size_ is the size of memory in **byte**s; ISO/IEC suffixes are recognised - e.g. `Ki` for units of 1024 **byte**s.
	this directive _must_ be present _before_ assembler instructions are processed.

`$NOCASE`
	ignore the character case of symbols.

#### identifier directives

Identifiers consist of the characters `_` and `$`, the letters `A` to `Z` and `a` to `z`, and the digits `0` to `9` which _may not_ be the initial character.

`{` _identifier_ `}`
	defines a keyword, the _identifier_ is retained in the pattern, and _no_ field assignment is made.

`{` _identifier_ `:` _replacement_ `=` _value_ `}`
	defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and _value_ is assigned to the next available field.

`{` _identifier_ `:` _replacement_ `}`
	defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and a sequentially calaculated value is assigned to the next available field.

#### set directives

`{` _identifier_ `#` _replacement_ `}`
	defines a common _replacement_ for a *set* of _identifiers_; when processing an **identifier directive**, the _replacement_ field is first look for in the **set** table and the _replacement_ from the **set** member is used instead.
	**set**s maintain independant enumeration values for identifier value assignment.
	**set** member data can be exported to a C header file via the `-s` command line option:
		for each **set** member as a `#define` _set-identifier_`_`_member-identifier_ _member-value_
		and an array _set-identifier_`_name` of _member-identifier_ strings.

#### function directives

`{` _identifier_ `{` _expression_ `}` `}`
	_expression_ is evaluated when _identifier_ is encountered when evaluating an expression.

### patterns

_patterns_ are a sequence of graphical characters excluding the special characters mentioned above.

_pattern_ `{` _expression_ `}` [ `:` `{` _link-expression_ `}` ]
	_expression_ is evaluated whenever _pattern_ is matched in an assembly source file.
	_link-expression_ is an optional expression to be evaluated when resolving symbol addresses; it will not be evaluated if the prior evaluation of _expression_ returns a zero value. If _link-expression_ is to be evaluated, the values of the field variants prior to the execution of _expression_ are preserved.

### expressions

Expressions are enclosed by a `'{' ... '}' pair.

#### constants

##### named constants

`TRUE` or `true`
	returns `1`

`FALSE` or `false`
	returns `0`

##### integers

C style representation.

#### variants

Variants are an initial `$` or `@`, followed by an integer number; `$` variants are used to directly access a value, '@' variants are used when accessing an address value via a symbol.

Although not implicitly asssigned, the `$0` variant is intended for use as the current address.

When parsing instructions, operand values are sequentially assigned to variants starting at `$1`.

#### operators

Operators in order of increasing precedence are:

##### sequence

_left_ `,` _right_
	divides an expression into sub expressions that are evaluated in left-to-right order.

##### assignment

_address_ `@=` _value_
	is the **emit** operator; it writes a single **byte** from the _value_ sub-expression to the memory at the _address_ sub-expression.

_variant_ `=` _value_
	assigns the _value_ sub-expression to a _variant_.

##### condition

_condition_ `?` _subsequent_ [ '!' _alternate_ ]
	evaluates the _condition_ expresssion and if the result is non-zero, evaluates the _subsquent_ expression; if it is zero, then the optional _alternate_ expression is evaluated.

##### boolean

_left_ `||` _right_
	evaluates the _left_ and _right_ expressions and returns `0` if both results are zero, `1` otherwise.

_left_ `&&` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if both results are non-zero, `0` otherwise.

##### relation

_left_ `==` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if the results are equal, `0` otherwise.

_left_ `!=` _right_
_left_ `<>` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if the results are not equal, `0` otherwise.

_left_ `<` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if _left_ is less than _right_, `0` otherwise.

_left_ `<=` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if _left_ is less than or equal to _right_, `0` otherwise.

_left_ `>` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if _left_ is more than _right_, `0` otherwise.

_left_ `>=` _right_
	evaluates the _left_ and _right_ expressions and returns `1` if _left_ is more than or equal to _right_, `0` otherwise.

##### bitwise

_left_ `|` _right_
	evaluates the _left_ and _right_ expressions and returns the bitwise OR of the results.

_left_ `&` _right_
	evaluates the _left_ and _right_ expressions and returns the bitwise AND of the results.

_left_ `^` _right_
	evaluates the _left_ and _right_ expressions and returns the bitwise XOR of the results.

##### additive

_left_ `+` _right_
	evaluates the _left_ and _right_ expressions and returns the sum of the results.

_left_ `-` _right_
	evaluates the _left_ and _right_ expressions and returns the result of subtracting _right_ from _left_.

##### multiplicative

_left_ `*` _right_
	evaluates the _left_ and _right_ expressions and returns the arithmetic product of the results.

_left_ `/` _right_
	evaluates the _left_ and _right_ expressions and returns the quotient resulting from the division of _left_ by _right_.

_left_ `%` _right_
	evaluates the _left_ and _right_ expressions and returns the remainder resulting from the division of _left_ by _right_.

##### exponentive

_left_ `<<` _right_
	evaluates the _left_ and _right_ expressions and returns the result of the bitwise shift _left_ left by _right_.

_left_ `>>` _right_
	evaluates the _left_ and _right_ expressions and returns the result of the bitwise shift _left_ right by _right_.

_left_ `<<>` _right_
	evaluates the _left_ and _right_ expressions and returns the result of the bitwise rotation _left_ left by _right_.

_left_ `<>>` _right_
	evaluates the _left_ and _right_ expressions and returns the result of the bitwise rotation _left_ right by _right_.

##### unary prefix

`?` _value_
	evaluates _value_ expression and returns '1' if the result is non-zero, `0` otherwise.

`!` _value_
	evaluates _value_ expression and returns '1' if the result is zero, `0` otherwise.

`|` _value_
	evaluates _value_ expression and returns the position of the most-siginificant bit (`1` for lowest bit), or `0` if no bit set.

`~` _value_
	evaluates _value_ expression and returns the bitwise complement.

`-` _value_
	evaluates _value_ expression and returns the negation of the result.

`+` _value_
	evaluates _value_ expression and returns its result.

##### unary suffix

_variant_ `++`
	increments the value of _variant_, returns the pre-increment value.

_variant_ `--`
	decrements the value of _variant_, returns the pre-decrement value.

##### primary

`(` _expression_ `)`
	evaluates and returns the result of _expression_.

_function_
	The defined _function_ expression is evaluated.

_identifier_
	The value assigned to (non-function) _identifier_ is returned.

`INVALID`
	outputs an invalid operand error message.
	returns `0`.

## assembly source files

The following characters have special meaning in assembler files: `#` `;` `,`.
`#` and `;` are as above, and `,` is used to separate multiple instructions on a single line.

Source lines are parsed, spaces are elided, symbols and constants are replaced by their defined replacement strings, stopping at the `,` separator, comments, or end-of-line. The resulting pattern is then looked up in the pattern table and if a match is found, the corresponding expression is evaluated.

## Building

Uses [HOL](https://github.com/stytri/hol) and [defer](https://github.com/stytri/defer).

Compile with [m](https://github.com/stytri/m).

