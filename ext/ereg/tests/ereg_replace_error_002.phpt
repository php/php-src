--TEST--
Test ereg_replace() function : error conditions - bad regular expressions
--FILE--
<?php
/* Prototype  : proto string ereg_replace(string pattern, string replacement, string string)
 * Description: Replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

echo "*** Testing ereg_replace() : bad REs ***\n";
var_dump(ereg_replace("", "hello", "some string"));
var_dump(ereg_replace("c(d", "hello", "some string"));
var_dump(ereg_replace("a[b", "hello", "some string"));
var_dump(ereg_replace("c(d", "hello", "some string"));;
var_dump(ereg_replace("*", "hello", "some string"));
var_dump(ereg_replace("+", "hello", "some string"));
var_dump(ereg_replace("?", "hello", "some string"));
var_dump(ereg_replace("(+?*)", "hello", "some string"));
var_dump(ereg_replace("h{256}", "hello", "some string"));
var_dump(ereg_replace("h|", "hello", "some string"));
var_dump(ereg_replace("h{0}", "hello", "some string"));
var_dump(ereg_replace("h{2,1}", "hello", "some string"));
var_dump(ereg_replace('[a-c-e]', 'd', "some string"));
var_dump(ereg_replace('\\', 'x', "some string"));
var_dump(ereg_replace('([9-0])', '1', "some string"));
echo "Done";
?>
--EXPECTF--
*** Testing ereg_replace() : bad REs ***

Warning: ereg_replace(): REG_EMPTY in %s on line 9
bool(false)

Warning: ereg_replace(): REG_EPAREN in %s on line 10
bool(false)

Warning: ereg_replace(): REG_EBRACK in %s on line 11
bool(false)

Warning: ereg_replace(): REG_EPAREN in %s on line 12
bool(false)

Warning: ereg_replace(): REG_BADRPT in %s on line 13
bool(false)

Warning: ereg_replace(): REG_BADRPT in %s on line 14
bool(false)

Warning: ereg_replace(): REG_BADRPT in %s on line 15
bool(false)

Warning: ereg_replace(): REG_BADRPT in %s on line 16
bool(false)

Warning: ereg_replace(): REG_BADBR in %s on line 17
bool(false)

Warning: ereg_replace(): REG_EMPTY in %s on line 18
bool(false)

Warning: ereg_replace(): REG_EMPTY in %s on line 19
bool(false)

Warning: ereg_replace(): REG_BADBR in %s on line 20
bool(false)

Warning: ereg_replace(): REG_ERANGE in %s on line 21
bool(false)

Warning: ereg_replace(): REG_EESCAPE in %s on line 22
bool(false)

Warning: ereg_replace(): REG_ERANGE in %s on line 23
bool(false)
Done