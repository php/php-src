
# Oniguruma syntax (operator) configuration

_Documented for Oniguruma 6.9.3 (2019/08/08)_


----------


## Overview

This document details how to configure Oniguruma's syntax, by describing the desired
syntax operators and behaviors in an instance of the OnigSyntaxType struct, just like
the built-in Oniguruma syntaxes do.

Configuration operators are bit flags, and are broken into multiple groups, somewhat arbitrarily,
because Oniguruma takes its configuration as a trio of 32-bit `unsigned int` values, assigned as
the first three fields in an `OnigSyntaxType` struct:

```C
typedef struct {
  unsigned int   op;
  unsigned int   op2;
  unsigned int   behavior;
  OnigOptionType options;   /* default option */
  OnigMetaCharTableType meta_char_table;
} OnigSyntaxType;
```

The first group of configuration flags (`op`) roughly corresponds to the
configuration for "basic regex."  The second group (`op2`) roughly corresponds
to the configuration for "advanced regex."  And the third group (`behavior`)
describes more-or-less what to do for broken input, bad input, or other corner-case
regular expressions whose meaning is not well-defined.  These three groups of
flags are described in full below, and tables of their usages for various syntaxes
follow.

The `options` field describes the default compile options to use if the caller does
not specify any options when invoking `onig_new()`.

The `meta_char_table` field is used exclusively by the ONIG_SYN_OP_VARIABLE_META_CHARACTERS
option, which allows the various regex metacharacters, like `*` and `?`, to be replaced
with alternates (for example, SQL typically uses `%` instead of `.*` and `_` instead of `?`).


----------


## Group One Flags (op)


This group contains "basic regex" constructs, features common to most regex systems.


### 0. ONIG_SYN_OP_VARIABLE_META_CHARACTERS

_Set in: none_

Enables support for `onig_set_meta_char()`, which allows you to provide alternate
characters that will be used instead of the six special characters that are normally
these characters below:

   - `ONIG_META_CHAR_ESCAPE`: `\`
   - `ONIG_META_CHAR_ANYCHAR`: `.`
   - `ONIG_META_CHAR_ANYTIME`: `*`
   - `ONIG_META_CHAR_ZERO_OR_ONE_TIME`: `?`
   - `ONIG_META_CHAR_ONE_OR_MORE_TIME`: `+`
   - `ONIG_META_CHAR_ANYCHAR_ANYTIME`: Equivalent in normal regex to `.*`, but supported
      explicitly so that Oniguruma can support matching SQL `%` wildcards or shell `*` wildcards.

If this flag is set, then the values defined using `onig_set_meta_char()` will be used;
if this flag is clear, then the default regex characters will be used instead, and
data set by `onig_set_meta_char()` will be ignored.


### 1. ONIG_SYN_OP_DOT_ANYCHAR (enable `.`)

_Set in: PosixBasic, PosixExtended, Emacs, Grep, GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for the standard `.` metacharacter, meaning "any one character."  You
usually want this flag on unless you have turned on `ONIG_SYN_OP_VARIABLE_META_CHARACTERS`
so that you can use a metacharacter other than `.` instead.


### 2. ONIG_SYN_OP_ASTERISK_ZERO_INF (enable `r*`)

_Set in: PosixBasic, PosixExtended, Emacs, Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the standard `r*` metacharacter, meaning "zero or more r's."
You usually want this flag set unless you have turned on `ONIG_SYN_OP_VARIABLE_META_CHARACTERS`
so that you can use a metacharacter other than `*` instead.


### 3. ONIG_SYN_OP_ESC_ASTERISK_ZERO_INF (enable `r\*`)

_Set in: none_

Enables support for an escaped `r\*` metacharacter, meaning "zero or more r's."  This is
useful if you have disabled support for the normal `r*` metacharacter because you want `*`
to simply match a literal `*` character, but you still want some way of activating "zero or more"
behavior.


### 4. ONIG_SYN_OP_PLUS_ONE_INF (enable `r+`)

_Set in: PosixExtended, Emacs, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the standard `r+` metacharacter, meaning "one or more r's."
You usually want this flag set unless you have turned on `ONIG_SYN_OP_VARIABLE_META_CHARACTERS`
so that you can use a metacharacter other than `+` instead.


### 5. ONIG_SYN_OP_ESC_PLUS_ONE_INF (enable `r\+`)

_Set in: Grep_

Enables support for an escaped `r\+` metacharacter, meaning "one or more r's."  This is
useful if you have disabled support for the normal `r+` metacharacter because you want `+`
to simply match a literal `+` character, but you still want some way of activating "one or more"
behavior.


### 6. ONIG_SYN_OP_QMARK_ZERO_ONE (enable `r?`)

_Set in: PosixExtended, Emacs, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the standard `r?` metacharacter, meaning "zero or one r" or "an optional r."
You usually want this flag set unless you have turned on `ONIG_SYN_OP_VARIABLE_META_CHARACTERS`
so that you can use a metacharacter other than `?` instead.


### 7. ONIG_SYN_OP_ESC_QMARK_ZERO_ONE (enable `r\?`)

_Set in: Grep_

Enables support for an escaped `r\?` metacharacter, meaning "zero or one r" or "an optional
r."  This is useful if you have disabled support for the normal `r?` metacharacter because
you want `?` to simply match a literal `?` character, but you still want some way of activating
"optional" behavior.


### 8. ONIG_SYN_OP_BRACE_INTERVAL (enable `r{l,u}`)

_Set in: PosixExtended, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the `r{lower,upper}` range form, common to more advanced
regex engines, which lets you specify precisely a minimum and maximum range on how many r's
must match (and not simply "zero or more").

This form also allows `r{count}` to specify a precise count of r's that must match.

This form also allows `r{lower,}` to be equivalent to `r{lower,infinity}`.

If and only if the `ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV` behavior flag is set,
this form also allows `r{,upper}` to be equivalent to `r{0,upper}`; otherwise,
`r{,upper}` will be treated as an error.


### 9. ONIG_SYN_OP_ESC_BRACE_INTERVAL (enable `\{` and `\}`)

_Set in: PosixBasic, Emacs, Grep_

Enables support for an escaped `r\{lower,upper\}` range form.  This is useful if you
have disabled support for the normal `r{...}` range form and want curly braces to simply
match literal curly brace characters, but you still want some way of activating
"range" behavior.


### 10. ONIG_SYN_OP_VBAR_ALT (enable `r|s`)

_Set in: PosixExtended, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `r|s` alternation operator.  You usually want this
flag set.


### 11. ONIG_SYN_OP_ESC_VBAR_ALT (enable `\|`)

_Set in: Emacs, Grep_

Enables support for an escaped `r\|s` alternation form.  This is useful if you
have disabled support for the normal `r|s` alternation form and want `|` to simply
match a literal `|` character, but you still want some way of activating "alternate" behavior.


### 12. ONIG_SYN_OP_LPAREN_SUBEXP (enable `(r)`)

_Set in: PosixExtended, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `(...)` grouping-and-capturing operators.  You usually
want this flag set.


### 13. ONIG_SYN_OP_ESC_LPAREN_SUBEXP (enable `\(` and `\)`)

_Set in: PosixBasic, Emacs, Grep_

Enables support for escaped `\(...\)` grouping-and-capturing operators.  This is useful if you
have disabled support for the normal `(...)` grouping-and-capturing operators and want
parentheses to simply match literal parenthesis characters, but you still want some way of
activating "grouping" or "capturing" behavior.


### 14. ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR (enable `\A` and `\Z` and `\z`)

_Set in: GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the anchors `\A` (start-of-string), `\Z` (end-of-string or
newline-at-end-of-string), and `\z` (end-of-string) escapes.

(If the escape metacharacter has been changed from the default of `\`, this
option will recognize that metacharacter instead.)


### 15. ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR (enable `\G`)

_Set in: GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the special anchor `\G` (start-of-previous-match).

(If the escape metacharacter has been changed from the default of `\`, this
option will recognize that metacharacter instead.)

Note that `OnigRegex`/`regex_t` are not stateful objects, and do _not_ record
the location of the previous match.  The `\G` flag uses the `start` parameter
explicitly passed to `onig_search()` (or `onig_search_with_param()` to determine
the "start of the previous match," so if the caller always passes the start of
the entire buffer as the function's `start` parameter, then `\G` will behave
exactly the same as `\A`.


### 16. ONIG_SYN_OP_DECIMAL_BACKREF (enable `\num`)

_Set in: PosixBasic, PosixExtended, Emacs, Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for subsequent matches to back references to prior capture groups `(...)` using
the common `\num` syntax (like `\3`).

If this flag is clear, then a numeric escape like `\3` will either be treated as a literal `3`,
or, if `ONIG_SYN_OP_ESC_OCTAL3` is set, will be treated as an octal character code `\3`.

You usually want this enabled, and it is enabled by default in every built-in syntax.


### 17. ONIG_SYN_OP_BRACKET_CC (enable `[...]`)

_Set in: PosixBasic, PosixExtended, Emacs, Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for recognizing character classes, like `[a-z]`.  If this flag is not set, `[`
and `]` will be treated as ordinary literal characters instead of as metacharacters.

You usually want this enabled, and it is enabled by default in every built-in syntax.


### 18. ONIG_SYN_OP_ESC_W_WORD (enable `\w` and `\W`)

_Set in: Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `\w` and `\W` shorthand forms.  These match "word characters,"
whose meaning varies depending on the encoding being used.

In ASCII encoding, `\w` is equivalent to `[A-Za-z0-9_]`.

In most other encodings, `\w` matches many more characters, including accented letters, Greek letters,
Cyrillic letters, Braille letters and numbers, Runic letters, Hebrew letters, Arabic letters and numerals,
Chinese Han ideographs, Japanese Katakana and Hiragana, Korean Hangul, and generally any symbol that
could qualify as a phonetic "letter" or counting "number" in any language.  (Note that emoji are _not_
considered "word characters.")

`\W` always matches the opposite of whatever `\w` matches.


### 19. ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END (enable `\<` and `\>`)

_Set in: Grep, GnuRegex_

Enables support for the GNU-specific `\<` and `\>` word-boundary metacharacters.  These work like
the `\b` word-boundary metacharacter, but only match at one end of the word or the other:  `\<`
only matches at a transition from a non-word character to a word character (i.e., at the start
of a word), and `\>` only matches at a transition from a word character to a non-word character
(i.e., at the end of a word).

Most regex syntaxes do _not_ support these metacharacters.


### 20. ONIG_SYN_OP_ESC_B_WORD_BOUND (enable `\b` and `\B`)

_Set in: Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `\b` and `\B` word-boundary metacharacters.  The `\b` metacharacter
matches a zero-width position at a transition from word-characters to non-word-characters, or vice
versa.  The `\B` metacharacter matches at all positions _not_ matched by `\b`.

See details in `ONIG_SYN_OP_ESC_W_WORD` above for an explanation as to which characters
are considered "word characters."


### 21. ONIG_SYN_OP_ESC_S_WHITE_SPACE (enable `\s` and `\S`)

_Set in: GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `\s` and `\S` whitespace-matching metacharacters.

The `\s` metacharacter in ASCII encoding is exactly equivalent to the character class
`[\t\n\v\f\r ]`, or characters codes 9 through 13 (inclusive), and 32.

The `\s` metacharacter in Unicode is exactly equivalent to the character class
`[\t\n\v\f\r \x85\xA0\x1680\x2000-\x200A\x2028-\x2029\x202F\x205F\x3000]` — that is, it matches
the same as ASCII, plus U+0085 (next line), U+00A0 (nonbreaking space), U+1680 (Ogham space mark),
U+2000 (en quad) through U+200A (hair space) (this range includes several widths of Unicode spaces),
U+2028 (line separator) through U+2029 (paragraph separator),
U+202F (narrow no-break space), U+205F (medium mathematical space), and U+3000 (CJK ideographic space).

All non-Unicode encodings are handled by converting their code points to the appropriate
Unicode-equivalent code points, and then matching according to Unicode rules.

`\S` always matches any one character that is _not_ in the set matched by `\s`.


### 22. ONIG_SYN_OP_ESC_D_DIGIT (enable `\d` and `\D`)

_Set in: GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `\d` and `\D` digit-matching metacharacters.

The `\d` metacharacter in ASCII encoding is exactly equivalent to the character class
`[0-9]`, or characters codes 48 through 57 (inclusive).

The `\d` metacharacter in Unicode matches `[0-9]`, as well as digits in Arabic, Devanagari,
Bengali, Laotian, Mongolian, CJK fullwidth numerals, and many more.

All non-Unicode encodings are handled by converting their code points to the appropriate
Unicode-equivalent code points, and then matching according to Unicode rules.

`\D` always matches any one character that is _not_ in the set matched by `\d`.


### 23. ONIG_SYN_OP_LINE_ANCHOR (enable `^r` and `r$`)

_Set in: Emacs, Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the common `^` and `$` line-anchor metacharacters.

In single-line mode, `^` matches the start of the input buffer, and `$` matches
the end of the input buffer.  In multi-line mode, `^` matches if the preceding
character is `\n`; and `$` matches if the following character is `\n`.

(Note that Oniguruma does not recognize other newline types:  It only matches
`^` and `$` against `\n`:  not `\r`, not `\r\n`, not the U+2028 line separator,
and not any other form.)


### 24. ONIG_SYN_OP_POSIX_BRACKET (enable POSIX `[:xxxx:]`)

_Set in: PosixBasic, PosixExtended, Grep, GnuRegex, Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for the POSIX `[:xxxx:]` character classes, like `[:alpha:]` and `[:digit:]`.
The supported POSIX character classes are `alnum`, `alpha`, `blank`, `cntrl`, `digit`,
`graph`, `lower`, `print`, `punct`, `space`, `upper`, `xdigit`, `ascii`, `word`.


### 25. ONIG_SYN_OP_QMARK_NON_GREEDY (enable `r??`, `r*?`, `r+?`, and `r{n,m}?`)

_Set in: Perl, Java, Perl_NG, Ruby, Oniguruma_

Enables support for lazy (non-greedy) quantifiers: That is, if you append a `?` after
another quantifier such as `?`, `*`, `+`, or `{n,m}`, Oniguruma will try to match
as _little_ as possible instead of as _much_ as possible.


### 26. ONIG_SYN_OP_ESC_CONTROL_CHARS (enable `\n`, `\r`, `\t`, etc.)

_Set in: PosixBasic, PosixExtended, Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for C-style control-code escapes, like `\n` and `\r`.  Specifically,
this recognizes `\a` (7), `\b` (8), `\t` (9), `\n` (10), `\f` (12), `\r` (13), and
`\e` (27).  If ONIG_SYN_OP2_ESC_V_VTAB is enabled (see below), this also enables
support for recognizing `\v` as code point 11.


### 27. ONIG_SYN_OP_ESC_C_CONTROL (enable `\cx` control codes)

_Set in: Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for named control-code escapes, like `\cm` or `\cM` for code-point
13.  In this shorthand form, control codes may be specified by `\c` (for "Control")
followed by an alphabetic letter, a-z or A-Z, indicating which code point to represent
(1 through 26).  So `\cA` is code point 1, and `\cZ` is code point 26.


### 28. ONIG_SYN_OP_ESC_OCTAL3 (enable `\OOO` octal codes)

_Set in: Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for octal-style escapes of up to three digits, like `\1` for code
point 1, and `\177` for code point 127.  Octal values greater than 255 will result
in an error message.


### 29. ONIG_SYN_OP_ESC_X_HEX2 (enable `\xHH` hex codes)

_Set in: Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for hexadecimal-style escapes of up to two digits, like `\x1` for code
point 1, and `\x7F` for code point 127.


### 30. ONIG_SYN_OP_ESC_X_BRACE_HEX8 (enable `\x{7HHHHHHH}` hex codes)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

Enables support for brace-wrapped hexadecimal-style escapes of up to eight digits,
like `\x{1}` for code point 1, and `\x{FFFE}` for code point 65534.


### 31. ONIG_SYN_OP_ESC_O_BRACE_OCTAL (enable `\o{1OOOOOOOOOO}` octal codes)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

Enables support for brace-wrapped octal-style escapes of up to eleven digits,
like `\o{1}` for code point 1, and `\o{177776}` for code point 65534.

(New feature as of Oniguruma 6.3.)


----------


## Group Two Flags (op2)


This group contains support for lesser-known regex syntax constructs.


### 0. ONIG_SYN_OP2_ESC_CAPITAL_Q_QUOTE (enable `\Q...\E`)

_Set in: Java, Perl, Perl_NG_

Enables support for "quoted" parts of a pattern:  Between `\Q` and `\E`, all
syntax parsing is turned off, so that metacharacters like `*` and `+` will no
longer be treated as metacharacters, and instead will be matched as literal
`*` and `+`, as if they had been escaped with `\*` and `\+`.


### 1. ONIG_SYN_OP2_QMARK_GROUP_EFFECT (enable `(?...)`)

_Set in: Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for the fairly-common `(?...)` grouping operator, which
controls precedence but which does _not_ capture its contents.


### 2. ONIG_SYN_OP2_OPTION_PERL (enable options `(?imsx)` and `(?-imsx)`)

_Set in: Java, Perl, Perl_NG_

Enables support of regex options. (i,m,s,x)
The supported toggle-able options for this flag are:

  - `i` - Case-insensitivity
  - `m` - Multi-line mode (`^` and `$` match at `\n` as well as start/end of buffer)
  - `s` - Single-line mode (`.` can match `\n`)
  - `x` - Extended pattern (free-formatting: whitespace will ignored)


### 3. ONIG_SYN_OP2_OPTION_RUBY (enable options `(?imx)` and `(?-imx)`)

_Set in: Ruby, Oniguruma_

Enables support of regex options. (i,m,x)
The supported toggle-able options for this flag are:

  - `i` - Case-insensitivity
  - `m` - Multi-line mode (`.` can match `\n`)
  - `x` - Extended pattern (free-formatting: whitespace will ignored)


### 4. ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT (enable `r?+`, `r*+`, and `r++`)

_Set in: Ruby, Oniguruma_

Enables support for the _possessive_ quantifiers `?+`, `*+`, and `++`, which
work similarly to `?` and `*` and `+`, respectively, but which do not backtrack
after matching:  Like the normal greedy quantifiers, they match as much as
possible, but they do not attempt to match _less_ than their maximum possible
extent if subsequent parts of the pattern fail to match.


### 5. ONIG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL (enable `r{n,m}+`)

_Set in: Java_

Enables support for the _possessive_ quantifier `{n,m}+`, which
works similarly to `{n,m}`, but which does not backtrack
after matching:  Like the normal greedy quantifier, it matches as much as
possible, but it do not attempt to match _less_ than its maximum possible
extent if subsequent parts of the pattern fail to match.


### 6. ONIG_SYN_OP2_CCLASS_SET_OP (enable `&&` within `[...]`)

_Set in: Java, Ruby, Oniguruma_

Enables support for character-class _intersection_.  For example, with this
feature enabled, you can write `[a-z&&[^aeiou]]` to produce a character class
of only consonants, or `[\0-\37&&[^\n\r]]` to produce a character class of
all control codes _except_ newlines.


### 7. ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP (enable named captures `(?<name>...)`)

_Set in: Perl_NG, Ruby, Oniguruma_

Enables support for _naming_ capture groups, so that instead of having to
refer to captures by position (like `\3` or `$3`), you can refer to them by names
(like `server` and `path`).  This supports the Perl/Ruby naming syntaxes `(?<name>...)`
and `(?'name'...)`, but not the Python `(?P<name>...)` syntax.


### 8. ONIG_SYN_OP2_ESC_K_NAMED_BACKREF (enable named backreferences `\k<name>`)

_Set in: Perl_NG, Ruby, Oniguruma_

Enables support for substituted backreferences by name, not just by position.
This supports using `\k'name'` in addition to supporting `\k<name>`.  This also
supports an Oniguruma-specific extension that lets you specify the _distance_ of
the match, if the capture matched multiple times, by writing `\k<name+n>` or
`\k<name-n>`.


### 9. ONIG_SYN_OP2_ESC_G_SUBEXP_CALL (enable backreferences `\g<name>` and `\g<n>`)

_Set in: Perl_NG, Ruby, Oniguruma_

Enables support for substituted backreferences by both name and position using
the same syntax.  This supports using `\g'name'` and `\g'1'` in addition to
supporting `\g<name>` and `\g<1>`.


### 10. ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY (enable `(?@...)` and `(?@<name>...)`)

_Set in: none_

Enables support for _capture history_, which can answer via the `onig_*capture*()`
functions exactly which captures were matched, how many times, and where in the
input they were matched, by placing `?@` in front of the capture.  Per Oniguruma's
regex syntax documentation (appendix A-5):

`/(?@a)*/.match("aaa")` ==> `[<0-1>, <1-2>, <2-3>]`

This can require substantial memory, is primarily useful for debugging, and is not
enabled by default in any syntax.


### 11. ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL (enable `\C-x`)

_Set in: Ruby, Oniguruma_

Enables support for Ruby legacy control-code escapes, like `\C-m` or `\C-M` for code-point
13.  In this shorthand form, control codes may be specified by `\C-` (for "Control")
followed by a single character (or equivalent), indicating which code point to represent,
based on that character's lowest five bits.  So, like `\c`, you can represent code-point
10 with `\C-j`, but you can also represent it with `\C-*` as well.

See also ONIG_SYN_OP_ESC_C_CONTROL, which enables the more-common `\cx` syntax.


### 12. ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META (enable `\M-x`)

_Set in: Ruby, Oniguruma_

Enables support for Ruby legacy meta-code escapes.  When you write `\M-x`, Oniguruma
will match an `x` whose 8th bit is set (i.e., the character code of `x` will be or'ed
with `0x80`).  So, for example, you can match `\x81` using `\x81`, or you can write
`\M-\1`.  This is mostly useful when working with legacy 8-bit character encodings.


### 13. ONIG_SYN_OP2_ESC_V_VTAB (enable `\v` as vertical tab)

_Set in: Java, Ruby, Oniguruma_

Enables support for a C-style `\v` escape code, meaning "vertical tab."  If enabled,
`\v` will be equivalent to ASCII code point 11.


### 14. ONIG_SYN_OP2_ESC_U_HEX4 (enable `\uHHHH` for Unicode)

_Set in: Java, Ruby, Oniguruma_

Enables support for a Java-style `\uHHHH` escape code for representing Unicode
code-points by number, using up to four hexadecimal digits (up to `\uFFFF`).  So,
for example, `\u221E` will match an infinity symbol, `∞`.

For code points larger than four digits, like the emoji `🚡` (aerial tramway, or code
point U+1F6A1), you must either represent the character directly using an encoding like
UTF-8, or you must enable support for ONIG_SYN_OP_ESC_X_BRACE_HEX8 or
ONIG_SYN_OP_ESC_O_BRACE_OCTAL, which support more than four digits.

(New feature as of Oniguruma 6.7.)


### 15. ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR (enable ``\` `` and `\'` anchors)

_Set in: Emacs_

This flag makes the ``\` `` and `\'` escapes function identically to
`\A` and `\z`, respectively (when ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR is enabled).

These anchor forms are very obscure, and rarely supported by other regex libraries.


### 16. ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY (enable `\p{...}` and `\P{...}`)

_Set in: Java, Perl, Perl_NG, Ruby, Oniguruma_

Enables support for an alternate syntax for POSIX character classes; instead of
writing `[:alpha:]` when this is enabled, you can instead write `\p{alpha}`.

See also ONIG_SYN_OP_POSIX_BRACKET for the classic POSIX form.


### 17. ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT (enable `\p{^...}` and `\P{^...}`)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

Enables support for an alternate syntax for POSIX character classes; instead of
writing `[:^alpha:]` when this is enabled, you can instead write `\p{^alpha}`.

See also ONIG_SYN_OP_POSIX_BRACKET for the classic POSIX form.


### 18. ONIG_SYN_OP2_CHAR_PROPERTY_PREFIX_IS

_(not presently used)_


### 19. ONIG_SYN_OP2_ESC_H_XDIGIT (enable `\h` and `\H`)

_Set in: Ruby, Oniguruma_

Enables support for the Ruby-specific shorthand `\h` and `\H` metacharacters.
Somewhat like `\d` matches decimal digits, `\h` matches hexadecimal digits — that is,
characters in `[0-9a-fA-F]`.

`\H` matches the opposite of whatever `\h` matches.


### 20. ONIG_SYN_OP2_INEFFECTIVE_ESCAPE (disable `\`)

_Set in: As-is_

If set, this disables all escape codes, shorthands, and metacharacters that start
with `\` (or whatever the configured escape character is), allowing `\` to be treated
as a literal `\`.

You usually do not want this flag to be enabled.


### 21. ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE (enable `(?(...)then|else)`)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

Enables support for conditional inclusion of subsequent regex patterns based on whether
a prior named or numbered capture matched, or based on whether a pattern will
match.  This supports many different forms, including:

  - `(?(<foo>)then|else)` - condition based on a capture by name.
  - `(?('foo')then|else)` - condition based on a capture by name.
  - `(?(3)then|else)` - condition based on a capture by number.
  - `(?(+3)then|else)` - forward conditional to a future match, by relative position.
  - `(?(-3)then|else)` - backward conditional to a prior match, by relative position.
  - `(?(foo)then|else)` - this matches a pattern `foo`. (foo is any sub-expression)

(New feature as of Oniguruma 6.5.)


### 22. ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP (enable `\K`)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

Enables support for `\K`, which excludes all content before it from the overall
regex match (i.e., capture #0).  So, for example, pattern `foo\Kbar` would match
`foobar`, but capture #0 would only include `bar`.

(New feature as of Oniguruma 6.5.)


### 23. ONIG_SYN_OP2_ESC_CAPITAL_R_GENERAL_NEWLINE (enable `\R`)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

Enables support for `\R`, the "general newline" shorthand, which matches
`(\r\n|[\n\v\f\r\u0085\u2028\u2029])` (obviously, the Unicode values are cannot be
matched in ASCII encodings).

(New feature as of Oniguruma 6.5.)


### 24. ONIG_SYN_OP2_ESC_CAPITAL_N_O_SUPER_DOT (enable `\N` and `\O`)

_Set in: Perl, Perl_NG, Oniguruma_

Enables support for `\N` and `\O`.  `\N` is "not a line break," which is much
like the standard `.` metacharacter, except that while `.` can be affected by
the single-line setting, `\N` always matches exactly one character that is not
one of the various line-break characters (like `\n` and `\r`).

`\O` matches exactly one character, regardless of whether single-line or
multi-line mode are enabled or disabled.

(New feature as of Oniguruma 6.5.)


### 25. ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP (enable `(?~...)`)

_Set in: Ruby, Oniguruma_

Enables support for the `(?~r)` "absent operator" syntax, which matches
as much as possible as long as the result _doesn't_ match pattern `r`.  This is
_not_ the same as negative lookahead or negative lookbehind.

Among the most useful examples of this is `\/\*(?~\*\/)\*\/`, which matches
C-style comments by simply saying "starts with /*, ends with */, and _doesn't_
contain a */ in between."

A full explanation of this feature is complicated, but it is useful, and an
excellent article about it is [available on Medium](https://medium.com/rubyinside/the-new-absent-operator-in-ruby-s-regular-expressions-7c3ef6cd0b99).

(New feature as of Oniguruma 6.5.)


### 26. ONIG_SYN_OP2_ESC_X_Y_TEXT_SEGMENT (enable `\X` and `\Y` and `\y`)

_Set in: Perl, Perl_NG, Ruby, Oniguruma_

`\X` is another variation on `.`, designed to support Unicode, in that it matches
a full _grapheme cluster_.  In Unicode, `à` can be encoded as one code point,
`U+00E0`, or as two, `U+0061 U+0300`.  If those are further escaped using UTF-8,
the former becomes two bytes, and the latter becomes three.  Unfortunately, `.`
would naively match only one or two bytes, depending on the encoding, and would
likely incorrectly match anything from just `a` to a broken half of a code point.
`\X` is designed to fix this:  It matches the full `à`, no matter how `à` is
encoded or decomposed.

`\y` matches a cluster boundary, i.e., a zero-width position between
graphemes, somewhat like `\b` matches boundaries between words.  `\Y` matches
the _opposite_ of `\y`, that is, a zero-width position between code points in
the _middle_ of a grapheme.

(New feature as of Oniguruma 6.6.)


### 27. ONIG_SYN_OP2_QMARK_PERL_SUBEXP_CALL (enable `(?R)` and `(?&name)`)

_Set in: Perl_NG_

Enables support for substituted backreferences by both name and position using
Perl-5-specific syntax.  This supports using `(?R3)` and `(?&name)` to reference
previous (and future) matches, similar to the more-common `\g<3>` and `\g<name>`
backreferences.

(New feature as of Oniguruma 6.7.)


### 28. ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS (enable `(?{...})`)

_Set in: Perl, Perl_NG, Oniguruma_

Enables support for Perl-style "callouts" — pattern substitutions that result from
invoking a callback method.  When `(?{foo})` is reached in a pattern, the callback
function set in `onig_set_progress_callout()` will be invoked, and be able to perform
custom computation during the pattern match (and during backtracking).

Full documentation for this advanced feature can be found in the Oniguruma
`docs/CALLOUT.md` file, with an example in `samples/callout.c`.

(New feature as of Oniguruma 6.8.)


### 29. ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME (enable `(*name)`)

_Set in: Perl, Perl_NG, Oniguruma_

Enables support for Perl-style "callouts" — pattern substitutions that result from
invoking a callback method.  When `(*foo)` is reached in a pattern, the callback
function set in `onig_set_callout_of_name()` will be invoked, passing the given name
`foo` to it, and it can perform custom computation during the pattern match (and
during backtracking).

Full documentation for this advanced feature can be found in the Oniguruma
`docs/CALLOUT.md` file, with an example in `samples/callout.c`.

(New feature as of Oniguruma 6.8.)


### 30. ONIG_SYN_OP2_OPTION_ONIGURUMA (enable options `(?imxWSDPy)` and `(?-imxWDSP)`)

_Set in: Oniguruma_

Enables support of regex options. (i,m,x,W,S,D,P,y)

(New feature as of Oniguruma 6.9.2)

  - `i` - Case-insensitivity
  - `m` - Multi-line mode (`.` can match `\n`)
  - `x` - Extended pattern (free-formatting: whitespace will ignored)
  - `W` - ASCII only word.
  - `D` - ASCII only digit.
  - `S` - ASCII only space.
  - `P` - ASCII only POSIX properties. (includes W,D,S)

----------


## Syntax Flags (syn)


This group contains rules to handle corner cases and constructs that are errors in
some syntaxes but not in others.

### 0. ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS (independent `?`, `*`, `+`, `{n,m}`)

_Set in: PosixExtended, GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

This flag specifies how to handle operators like `?` and `*` when they aren't
directly attached to an operand, as in `^*` or `(*)`:  Are they an error, are
they discarded, or are they taken as literals?  If this flag is clear, they
are taken as literals; otherwise, the ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS flag
determines if they are errors or if they are discarded.

### 1. ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS (error or ignore independent operators)

_Set in: PosixExtended, GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

If ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS is set, this flag controls what happens when
independent operators appear in a pattern:  If this flag is set, then independent
operators produce an error message; if this flag is clear, then independent
operators are silently discarded.

### 2. ONIG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP (allow `...)...`)

_Set in: PosixExtended_

This flag, if set, causes a `)` character without a preceding `(` to be treated as
a literal `)`, equivalent to `\)`.  If this flag is clear, then an unmatched `)`
character will produce an error message.

### 3. ONIG_SYN_ALLOW_INVALID_INTERVAL (allow `{???`)

_Set in: GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

This flag, if set, causes an invalid range, like `foo{bar}` or `foo{}`, to be
silently discarded, as if `foo` had been written instead.  If clear, an invalid
range will produce an error message.

### 4. ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV (allow `{,n}` to mean `{0,n}`)

_Set in: Ruby, Oniguruma_

If this flag is set, then `r{,n}` will be treated as equivalent to writing
`{0,n}`.  If this flag is clear, then `r{,n}` will produce an error message.

Note that regardless of whether this flag is set or clear, if
ONIG_SYN_OP_BRACE_INTERVAL is enabled, then `r{n,}` will always be legal:  This
flag *only* controls the behavior of the opposite form, `r{,n}`.

### 5. ONIG_SYN_STRICT_CHECK_BACKREF (error on invalid backrefs)

_Set in: none_

If this flag is set, an invalid backref, like `\1` in a pattern with no captures,
will produce an error.  If this flag is clear, then an invalid backref will be
equivalent to the empty string.

No built-in syntax has this flag enabled.

### 6. ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND (allow `(?<=a|bc)`)

_Set in: Java, Ruby, Oniguruma_

If this flag is set, lookbehind patterns with alternate options may have differing
lengths among those options.  If this flag is clear, lookbehind patterns with options
must have each option have identical length to the other options.

Oniguruma can handle either form, but not all regex engines can, so for compatibility,
Oniguruma allows you to cause regexes for other regex engines to fail if they might
depend on this rule.

### 7. ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP (prefer `\k<name>` over `\3`)

_Set in: Perl_NG, Ruby, Oniguruma_

If this flag is set on the syntax *and* ONIG_OPTION_CAPTURE_GROUP is set when calling
Oniguruma, then if a name is used on any capture, all captures must also use names:  A
single use of a named capture prohibits the use of numbered captures.

### 8. ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME (allow `(?<x>)...(?<x>)`)

_Set in: Perl_NG, Ruby, Oniguruma_

If this flag is set, multiple capture groups may use the same name.  If this flag is
clear, then reuse of a name will produce an error message.

### 9. ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY (`a{n}?` is equivalent to `(?:a{n})?`)

_Set in: Ruby, Oniguruma_

If this flag is set, then intervals of a fixed size will ignore a lazy (non-greedy)
`?` quantifier and treat it as an optional match (an ordinary `r?`), since "match as
little as possible" is meaningless for a fixed-size interval.  If this flag is clear,
then `r{n}?` will mean the same as `r{n}`, and the useless `?` will be discarded.

### 20. ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC (add `\n` to `[^...]`)

_Set in: Grep_

If this flag is set, all newline characters (like `\n`) will be excluded from a negative
character class automatically, as if the pattern had been written as `[^...\n]`.  If this
flag is clear, negative character classes do not automatically exclude newlines, and
only exclude those characters and ranges written in them.

### 21. ONIG_SYN_BACKSLASH_ESCAPE_IN_CC (allow `[...\w...]`)

_Set in: GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

If this flag is set, shorthands like `\w` are allowed to describe characters in character
classes.  If this flag is clear, shorthands like `\w` are treated as a redundantly-escaped
literal `w`.

### 22. ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC (silently discard `[z-a]`)

_Set in: Emacs, Grep_

If this flag is set, then character ranges like `[z-a]` that are broken or contain no
characters will be silently ignored.  If this flag is clear, then broken or empty
character ranges will produce an error message.

### 23. ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC (treat `[0-9-a]` as `[0-9\-a]`)

_Set in: PosixExtended, GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

If this flag is set, then a trailing `-` after a character range will be taken as a
literal `-`, as if it had been escaped as `\-`.  If this flag is clear, then a trailing
`-` after a character range will produce an error message.

### 24. ONIG_SYN_WARN_CC_OP_NOT_ESCAPED (warn on `[[...]` and `[-x]`)

_Set in: Ruby, Oniguruma_

If this flag is set, Oniguruma will be stricter about warning for bad forms in
character classes:  `[[...]` will produce a warning, but `[\[...]` will not;
`[-x]` will produce a warning, but `[\-x]` will not; `[x&&-y]` will produce a warning,
while `[x&&\-y]` will not; and so on.  If this flag is clear, all of these warnings
will be silently discarded.

### 25. ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT (warn on `(?:a*)+`)

_Set in: Ruby, Oniguruma_

If this flag is set, Oniguruma will warn about nested repeat operators those have no meaning, like `(?:a*)+`.
If this flag is clear, Oniguruma will allow the nested repeat operators without warning about them.

### 26. ONIG_SYN_ALLOW_INVALID_CODE_END_OF_RANGE_IN_CC (allow [a-\x{7fffffff}])

_Set in: Oniguruma_

If this flag is set, then invalid code points at the end of range in character class are allowed.

### 31. ONIG_SYN_CONTEXT_INDEP_ANCHORS

_Set in: PosixExtended, GnuRegex, Java, Perl, Perl_NG, Ruby, Oniguruma_

Not currently used, and does nothing.  (But still set in several syntaxes for some
reason.)

----------

## Usage tables

These tables show which of the built-in syntaxes use which flags and options, for easy comparison between them.

### Group One Flags (op)

| ID    | Option                                        | PosB  | PosEx | Emacs | Grep  | Gnu   | Java  | Perl  | PeNG  | Ruby  | Onig  |
| ----- | --------------------------------------------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| 0     | `ONIG_SYN_OP_VARIABLE_META_CHARACTERS`        | -     | -     | -     | -     | -     | -     | -     | -     | -     | -     |
| 1     | `ONIG_SYN_OP_DOT_ANYCHAR`                     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 2     | `ONIG_SYN_OP_ASTERISK_ZERO_INF`               | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 3     | `ONIG_SYN_OP_ESC_ASTERISK_ZERO_INF`           | -     | -     | -     | -     | -     | -     | -     | -     | -     | -     |
| 4     | `ONIG_SYN_OP_PLUS_ONE_INF`                    | -     | Yes   | Yes   | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 5     | `ONIG_SYN_OP_ESC_PLUS_ONE_INF`                | -     | -     | -     | Yes   | -     | -     | -     | -     | -     | -     |
| 6     | `ONIG_SYN_OP_QMARK_ZERO_ONE`                  | -     | Yes   | Yes   | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 7     | `ONIG_SYN_OP_ESC_QMARK_ZERO_ONE`              | -     | -     | -     | Yes   | -     | -     | -     | -     | -     | -     |
| 8     | `ONIG_SYN_OP_BRACE_INTERVAL`                  | -     | Yes   | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 9     | `ONIG_SYN_OP_ESC_BRACE_INTERVAL`              | Yes   | -     | Yes   | Yes   | -     | -     | -     | -     | -     | -     |
| 10    | `ONIG_SYN_OP_VBAR_ALT`                        | -     | Yes   | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 11    | `ONIG_SYN_OP_ESC_VBAR_ALT`                    | -     | -     | Yes   | Yes   | -     | -     | -     | -     | -     | -     |
| 12    | `ONIG_SYN_OP_LPAREN_SUBEXP`                   | -     | Yes   | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 13    | `ONIG_SYN_OP_ESC_LPAREN_SUBEXP`               | Yes   | -     | Yes   | Yes   | -     | -     | -     | -     | -     | -     |
| 14    | `ONIG_SYN_OP_ESC_AZ_BUF_ANCHOR`               | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 15    | `ONIG_SYN_OP_ESC_CAPITAL_G_BEGIN_ANCHOR`      | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 16    | `ONIG_SYN_OP_DECIMAL_BACKREF`                 | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 17    | `ONIG_SYN_OP_BRACKET_CC`                      | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 18    | `ONIG_SYN_OP_ESC_W_WORD`                      | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 19    | `ONIG_SYN_OP_ESC_LTGT_WORD_BEGIN_END`         | -     | -     | -     | Yes   | Yes   | -     | -     | -     | -     | -     |
| 20    | `ONIG_SYN_OP_ESC_B_WORD_BOUND`                | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 21    | `ONIG_SYN_OP_ESC_S_WHITE_SPACE`               | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 22    | `ONIG_SYN_OP_ESC_D_DIGIT`                     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 23    | `ONIG_SYN_OP_LINE_ANCHOR`                     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 24    | `ONIG_SYN_OP_POSIX_BRACKET`                   | Yes   | Yes   | Yes   | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 25    | `ONIG_SYN_OP_QMARK_NON_GREEDY`                | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 26    | `ONIG_SYN_OP_ESC_CONTROL_CHARS`               | Yes   | Yes   | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 27    | `ONIG_SYN_OP_ESC_C_CONTROL`                   | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 28    | `ONIG_SYN_OP_ESC_OCTAL3`                      | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 29    | `ONIG_SYN_OP_ESC_X_HEX2`                      | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 30    | `ONIG_SYN_OP_ESC_X_BRACE_HEX8`                | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |
| 31    | `ONIG_SYN_OP_ESC_O_BRACE_OCTAL`               | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |

### Group Two Flags (op2)

| ID    | Option                                        | PosB  | PosEx | Emacs | Grep  | Gnu   | Java  | Perl  | PeNG  | Ruby  | Onig  |
| ----- | --------------------------------------------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| 0     | `ONIG_SYN_OP2_ESC_CAPITAL_Q_QUOTE`            | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | -     | -     |
| 1     | `ONIG_SYN_OP2_QMARK_GROUP_EFFECT`             | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 2     | `ONIG_SYN_OP2_OPTION_PERL`                    | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | -     | -     |
| 3     | `ONIG_SYN_OP2_OPTION_RUBY`                    | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | -     |
| 4     | `ONIG_SYN_OP2_PLUS_POSSESSIVE_REPEAT`         | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 5     | `ONIG_SYN_OP2_PLUS_POSSESSIVE_INTERVAL`       | -     | -     | -     | -     | -     | Yes   | -     | -     | -     | -     |
| 6     | `ONIG_SYN_OP2_CCLASS_SET_OP`                  | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 7     | `ONIG_SYN_OP2_QMARK_LT_NAMED_GROUP`           | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 8     | `ONIG_SYN_OP2_ESC_K_NAMED_BACKREF`            | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 9     | `ONIG_SYN_OP2_ESC_G_SUBEXP_CALL`              | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 10    | `ONIG_SYN_OP2_ATMARK_CAPTURE_HISTORY`         | -     | -     | -     | -     | -     | -     | -     | -     | -     | -     |
| 11    | `ONIG_SYN_OP2_ESC_CAPITAL_C_BAR_CONTROL`      | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 12    | `ONIG_SYN_OP2_ESC_CAPITAL_M_BAR_META`         | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 13    | `ONIG_SYN_OP2_ESC_V_VTAB`                     | -     | -     | -     | -     | -     | Yes   | -     | -     | Yes   | Yes   |
| 14    | `ONIG_SYN_OP2_ESC_U_HEX4`                     | -     | -     | -     | -     | -     | Yes   | -     | -     | Yes   | Yes   |
| 15    | `ONIG_SYN_OP2_ESC_GNU_BUF_ANCHOR`             | -     | -     | Yes   | -     | -     | -     | -     | -     | -     | -     |
| 16    | `ONIG_SYN_OP2_ESC_P_BRACE_CHAR_PROPERTY`      | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   |
| 17    | `ONIG_SYN_OP2_ESC_P_BRACE_CIRCUMFLEX_NOT`     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |
| 18    | `ONIG_SYN_OP2_CHAR_PROPERTY_PREFIX_IS`        | -     | -     | -     | -     | -     | -     | -     | -     | -     | -     |
| 19    | `ONIG_SYN_OP2_ESC_H_XDIGIT`                   | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 20    | `ONIG_SYN_OP2_INEFFECTIVE_ESCAPE`             | -     | -     | -     | -     | -     | -     | -     | -     | -     | -     |
| 21    | `ONIG_SYN_OP2_QMARK_LPAREN_IF_ELSE`           | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |
| 22    | `ONIG_SYN_OP2_ESC_CAPITAL_K_KEEP`             | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |
| 23    | `ONIG_SYN_OP2_ESC_CAPITAL_R_GENERAL_NEWLINE`  | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |
| 24    | `ONIG_SYN_OP2_ESC_CAPITAL_N_O_SUPER_DOT`      | -     | -     | -     | -     | -     | -     | Yes   | Yes   | -     | Yes   |
| 25    | `ONIG_SYN_OP2_QMARK_TILDE_ABSENT_GROUP`       | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 26    | `ONIG_SYN_OP2_ESC_X_Y_TEXT_SEGMENT`           | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   |
| 27    | `ONIG_SYN_OP2_QMARK_PERL_SUBEXP_CALL`         | -     | -     | -     | -     | -     | -     | -     | Yes   | -     | -     |
| 28    | `ONIG_SYN_OP2_QMARK_BRACE_CALLOUT_CONTENTS`   | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | -     |
| 29    | `ONIG_SYN_OP2_ASTERISK_CALLOUT_NAME`          | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   | -     |
| 30    | `ONIG_SYN_OP2_OPTION_ONIGURUMA`               | -     | -     | -     | -     | -     | -     | -     | -     | -     | Yes   |

### Syntax Flags (syn)

| ID    | Option                                        | PosB  | PosEx | Emacs | Grep  | Gnu   | Java  | Perl  | PeNG  | Ruby  | Onig  |
| ----- | --------------------------------------------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| 0     | `ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS`           | -     | Yes   | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 1     | `ONIG_SYN_CONTEXT_INVALID_REPEAT_OPS`         | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 2     | `ONIG_SYN_ALLOW_UNMATCHED_CLOSE_SUBEXP`       | -     | Yes   | -     | -     | -     | -     | -     | -     | -     | -     |
| 3     | `ONIG_SYN_ALLOW_INVALID_INTERVAL`             | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 4     | `ONIG_SYN_ALLOW_INTERVAL_LOW_ABBREV`          | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 5     | `ONIG_SYN_STRICT_CHECK_BACKREF`               | -     | -     | -     | -     | -     | -     | -     | -     | -     | -     |
| 6     | `ONIG_SYN_DIFFERENT_LEN_ALT_LOOK_BEHIND`      | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 7     | `ONIG_SYN_CAPTURE_ONLY_NAMED_GROUP`           | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 8     | `ONIG_SYN_ALLOW_MULTIPLEX_DEFINITION_NAME`    | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   | Yes   |
| 9     | `ONIG_SYN_FIXED_INTERVAL_IS_GREEDY_ONLY`      | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 20    | `ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC`         | -     | -     | -     | Yes   | -     | -     | -     | -     | -     | -     |
| 21    | `ONIG_SYN_BACKSLASH_ESCAPE_IN_CC`             | -     | -     | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 22    | `ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC`            | -     | -     | Yes   | Yes   | -     | -     | -     | -     | -     | -     |
| 23    | `ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC`        | -     | Yes   | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
| 24    | `ONIG_SYN_WARN_CC_OP_NOT_ESCAPED`             | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 25    | `ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT`       | -     | -     | -     | -     | -     | -     | -     | -     | Yes   | Yes   |
| 26    | `ONIG_SYN_ALLOW_INVALID_CODE_END_OF_RANGE_IN_CC` | -     | -     | -     | -     | -     | -     | -     | -     | -     | Yes   |
| 31    | `ONIG_SYN_CONTEXT_INDEP_ANCHORS`              | -     | Yes   | -     | -     | Yes   | Yes   | Yes   | Yes   | Yes   | Yes   |
