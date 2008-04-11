--TEST--
Test eregi_replace() function : error conditions - bad regular expressions
--FILE--
<?php
/* Prototype  : proto string eregi_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

echo "*** Testing eregi_replace() : bad REs ***\n";
var_dump(eregi_replace(b"", "hello", "some string"));
var_dump(eregi_replace(b"c(d", "hello", "some string"));
var_dump(eregi_replace(b"a[b", "hello", "some string"));
var_dump(eregi_replace(b"c(d", "hello", "some string"));;
var_dump(eregi_replace(b"*", "hello", "some string"));
var_dump(eregi_replace(b"+", "hello", "some string"));
var_dump(eregi_replace(b"?", "hello", "some string"));
var_dump(eregi_replace(b"(+?*)", "hello", "some string"));
var_dump(eregi_replace(b"h{256}", "hello", "some string"));
var_dump(eregi_replace(b"h|", "hello", "some string"));
var_dump(eregi_replace(b"h{0}", "hello", "some string"));
var_dump(eregi_replace(b"h{2,1}", "hello", "some string"));
var_dump(eregi_replace(b'[a-c-e]', 'd', "some string"));
var_dump(eregi_replace(b'\\', 'x', "some string"));
var_dump(eregi_replace(b'([9-0])', '1', "some string"));
echo "Done";
?>
--EXPECTF--
*** Testing eregi_replace() : bad REs ***

Warning: eregi_replace(): REG_EMPTY in %s on line 9
bool(false)

Warning: eregi_replace(): REG_EPAREN in %s on line 10
bool(false)

Warning: eregi_replace(): REG_EBRACK in %s on line 11
bool(false)

Warning: eregi_replace(): REG_EPAREN in %s on line 12
bool(false)

Warning: eregi_replace(): REG_BADRPT in %s on line 13
bool(false)

Warning: eregi_replace(): REG_BADRPT in %s on line 14
bool(false)

Warning: eregi_replace(): REG_BADRPT in %s on line 15
bool(false)

Warning: eregi_replace(): REG_BADRPT in %s on line 16
bool(false)

Warning: eregi_replace(): REG_BADBR in %s on line 17
bool(false)

Warning: eregi_replace(): REG_EMPTY in %s on line 18
bool(false)

Warning: eregi_replace(): REG_EMPTY in %s on line 19
bool(false)

Warning: eregi_replace(): REG_BADBR in %s on line 20
bool(false)

Warning: eregi_replace(): REG_ERANGE in %s on line 21
bool(false)

Warning: eregi_replace(): REG_EESCAPE in %s on line 22
bool(false)

Warning: eregi_replace(): REG_ERANGE in %s on line 23
bool(false)
Done