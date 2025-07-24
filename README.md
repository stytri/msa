# msa

## Version 3.4.0

## Command Line

```
Matching String Assembler: a simple assembler for virtual machines.
usage: msa [OPTION]... [FILE]...
options:
  -h, --help                 display this help and exit
      --version              display version and exit
      --license              display license and exit
      --readme               display readme and exit
  -o, --output FILE          output to FILE
  -l, --listing FILE         write listing to FILE
  -s, --set-header FILE      write header FILE for sets
  -y, --symbols PFIX         include non-set symbols, adding prefix PFIX, in set header
  -a, --symfile FILE         write (addressable) symbol information to FILE
  -g, --segments SIZE        enable segmented memory - segments are allocated in blocks of SIZE bytes.
  -G, --fixed-segments SIZE  as -g, but segments are of fixed SIZE
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

&emsp;where _bits_ is one of `8` `16` `32` `64`; default is `8`.

`$BYTEFORMAT=` _format_

&emsp;where _format_ is one of `OCTAL` `DECIMAL` `HEXADECIMAL`

&emsp;used when printing numbers in listings; default is `HEXADECIMAL`.

`$ADDRESS=` _replacement_

&emsp;where _replacement_ is a sequence of graphical characters excluding `}`.

&emsp;when an address (i.e. undeclared symbol - see **identifier directives** below) is encountered in the assembly source, its is replaced by _replacement_ in the pattern.

&emsp;default is `%a`

`$INTEGER=` _replacement_

&emsp;where _replacement_ is a sequence of graphical characters excluding `}`.

&emsp;when an integer value is encountered in the assembly source, its is replaced by _replacement_ in the pattern.

&emsp;default is `%i`

`$SEGMENTS=` _size_ [`,` `FIXED`]

&emsp;where _size_ is the size of a segment in **byte**s; ISO/IEC suffixes are recognised (e.g. `Ki`) for units of 1024 **byte**s, as well as the vernacular `b` suffix (e.g. `KB`).

&emsp;if segments are to be enabled, this directive _must_ be present _before_ assembler instructions are processed.

&emsp;the optional `FIXED` argument forces the use of fixed size segments.

`$MEMORY=` _size_

&emsp;where _size_ is the size of memory in **byte**s; ISO/IEC suffixes are recognised (e.g. `Ki`) for units of 1024 **byte**s, as well as the vernacular `b` suffix (e.g. `KB`).

&emsp;this directive _must_ be present _before_ assembler instructions are processed.

`$NOCASE`

&emsp;ignore the character case of symbols.

#### identifier directives

Identifiers consist of the characters `_` and `$`, the letters `A` to `Z` and `a` to `z`, and the digits `0` to `9` which _may not_ be the initial character.
Additionally, the back-tick ( ` ) character can be used to quote the next [non-identifier] graphical character as part of the identifier;
**N.B.** _identifiers_ containing quoted characters are hidden from export.

Identifiers may be preceeded by an _attribute_, current attributes are:
-	`{delimeter}`  indicates that the _identifier_ is to be treated as an instruction delimeter.
-	`{hidden}`     indicates that the _identifier_ is to be excluded from export.

`{` [ _attribute_ ] _identifier_ `}`

&emsp;defines a keyword, the _identifier_ is retained in the pattern, and _no_ field assignment is made.

`{` [ _attribute_ ] _identifier_ `:` _replacement_ `=` _expression_ `}`

&emsp;defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and value of _expression_ is assigned to the next available field. Where _replacement_ is `$`, the _replacement_ is the same as _identifier_.

&emsp;**N.B.** _expression_ is evaluated immediately.

`{` [ _attribute_ ] _identifier_ `:` _replacement_ `}`

&emsp;defines an enumerated field value, the _identifier_ is replaced by _replacement_ in the pattern, and a sequentially calaculated value is assigned to the next available field. Where _replacement_ is `$`, the _replacement_ is the same as _identifier_.

#### set directives

`{` _identifier_ `#` _replacement_ [ `=` _expression_ ] `}`

&emsp;defines a common _replacement_ for a *set* of _identifiers_; when processing an **identifier directive**, the _replacement_ field is first look for in the **set** table and the _replacement_ from the **set** member is used instead.

&emsp;**set**s maintain independant enumeration values for identifier value assignment. Additionaly, the members of the **set** inherit the attributes of the **set**.

&emsp;**set** member data can be exported to a C header file via the `-s` command line option:

&emsp;	for each **set** member as a `#define` _set-identifier_`_`_member-identifier_ _member-value_

&emsp;	and an array _set-identifier_`_name` of _member-identifier_ strings.

&emsp;**N.B.** _expression_ is evaluated immediately.

#### function directives

`{` _identifier_ `{` _expression_ `}` `}`

&emsp;_expression_ is evaluated when _identifier_ is encountered when evaluating an expression.

### patterns

_patterns_ are a sequence of graphical characters excluding the special characters mentioned above.

_pattern_ `{` _expression_ `}` [ `:` `{` _link-expression_ `}` ]

&emsp;_expression_ is evaluated whenever _pattern_ is matched in an assembly source file.

&emsp;_link-expression_ is an optional expression to be evaluated when resolving symbol addresses; it will not be evaluated if the prior evaluation of _expression_ returns a zero value. If _link-expression_ is to be evaluated, the values of the field variants prior to the execution of _expression_ are preserved.

### expressions

Expressions are enclosed by a `{` ... `}` pair.

#### constants

##### named constants

`TRUE` or `true`

&emsp;returns `1`

`FALSE` or `false`

&emsp;returns `0`

##### integers

C style representation.

#### variants

Variants are an initial `$` or `@`, followed by an integer number; `$` variants are used to directly access a value, `@` variants are used when accessing an address value via a symbol.

Although not implicitly asssigned, the `$0` variant is intended for use as the current address.

When parsing instructions, operand values are sequentially assigned to variants starting at `$1`.

#### operators

Operators in order of increasing precedence are:

##### sequence

_left_ `,` _right_

&emsp;divides an expression into sub expressions that are evaluated in left-to-right order.

##### assignment

_address_ `@=` _value_

&emsp;is the **emit** operator; it writes a single **byte** from the _value_ sub-expression to the memory at the _address_ sub-expression.

_variant_ `=` _value_

&emsp;assigns the _value_ sub-expression to a _variant_.

##### condition

_condition_ `?` _subsequent_ [ `!` _alternate_ ]

&emsp;evaluates the _condition_ expresssion and if the result is non-zero, evaluates the _subsquent_ expression; if it is zero, then the optional _alternate_ expression is evaluated.

##### boolean

_left_ `||` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `0` if both results are zero, `1` otherwise.

_left_ `&&` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if both results are non-zero, `0` otherwise.

##### relation

_left_ `==` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if the results are equal, `0` otherwise.

_left_ `!=` _right_
_left_ `<>` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if the results are not equal, `0` otherwise.

_left_ `<` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is less than _right_, `0` otherwise.

_left_ `<=` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is less than or equal to _right_, `0` otherwise.

_left_ `>` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is more than _right_, `0` otherwise.

_left_ `>=` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns `1` if _left_ is more than or equal to _right_, `0` otherwise.

##### bitwise

_left_ `|` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the bitwise OR of the results.

_left_ `&` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the bitwise AND of the results.

_left_ `^` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the bitwise XOR of the results.

##### additive

_left_ `+` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the sum of the results.

_left_ `-` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the result of subtracting _right_ from _left_.

##### multiplicative

_left_ `*` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the arithmetic product of the results.

_left_ `/` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the quotient resulting from the division of _left_ by _right_.

_left_ `%` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the remainder resulting from the division of _left_ by _right_.

##### exponentive

_left_ `<<` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise shift _left_ left by _right_.

_left_ `>>` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise shift _left_ right by _right_.

_left_ `<<>` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise rotation _left_ left by _right_.

_left_ `<>>` _right_

&emsp;evaluates the _left_ and _right_ expressions and returns the result of the bitwise rotation _left_ right by _right_.

##### unary prefix

`$?`_variant_

&emsp;where _variant_ is the variant number; returns the type identifier of the _variant_.

`=@` _address_

&emsp;evaluates _address_ expression and returns the byte from memory at the resulting _address_.

`@&`_variant_ _value_

&emsp;evaluates _value_ expression and returns the bitwise and with the symbol _variant_'s _tag_ value.

`$&` _value_

&emsp;evaluates _value_ expression and returns the bitwise and with a global _tag_ value.

`$=` _value_

&emsp;evaluates _value_ expression and assigns to a global _tag_ value.

`?` _value_

&emsp;evaluates _value_ expression and returns `1` if the result is non-zero, `0` otherwise.

`!` _value_

&emsp;evaluates _value_ expression and returns `1` if the result is zero, `0` otherwise.

`|` _value_

&emsp;evaluates _value_ expression and returns the position of the most-siginificant bit (`1` for lowest bit), or `0` if no bit set.

`~` _value_

&emsp;evaluates _value_ expression and returns the bitwise complement.

`-` _value_

&emsp;evaluates _value_ expression and returns the negation of the result.

`+` _value_

&emsp;evaluates _value_ expression and returns its result.

##### unary suffix

_variant_ `++`

&emsp;increments the value of _variant_, returns the pre-increment value.

_variant_ `--`

&emsp;decrements the value of _variant_, returns the pre-decrement value.

##### primary

`(` _expression_ `)`

&emsp;evaluates and returns the result of _expression_.

_function_

&emsp;The defined _function_ expression is evaluated.

_identifier_

&emsp;The value assigned to (non-function) _identifier_ is returned.

`INVALID`

&emsp;outputs an invalid operand error message.

&emsp;returns `0`.

## assembly source files

The following characters have special meaning in assembler files: `#` `;` `,`.
`#` and `;` are as above, and `,` is used to separate multiple instructions on a single line.

Source lines are parsed, spaces are elided, symbols and constants are replaced by their defined replacement strings, stopping at the `,` or `:` separators (the colon separator is preserved in the pattern, the comma is elided), comments, or end-of-line. The resulting pattern is then looked up in the pattern table and if a match is found, the corresponding expression is evaluated.

Inside parenthesis (`(`...`)` or, `[`...`]`) delimeters and comments are inhibited, and the characters can be used as part of the pattern.
Parenthesis handling is _very_ basic; `(` and `[` increase the nesting level,  `]` and `)` decrease it; there is no balancing or, matching of the parenthesis characters.

## segfile

With version 3.0.0 `segfile.h` was introduced as a method writing and reading a file as a sequence of segments; this header should be copied/moved to a common include directory.

When enabled via the command line, segments are created automatically by **msa**; they have no attributes other than addresss and length.
* with the `-g` command line option, segments are dynamically expanded and merged.
* with the `-G` command line option, segments have a fixed size.

## symfile

With version 2.0.0 `symfile.h` was introduced as a method of passing symbol information to programs compiled with **msa**; this header should be copied/moved to a common include directory.

The current version of symfile (0) only supports symbol name, information (lower 32 bits of the global _tag_), and address, although provision is made for adding more data.

## Building

Uses [HOL](https://github.com/stytri/hol) and [defer](https://github.com/stytri/defer).

Compile with [m](https://github.com/stytri/m).

