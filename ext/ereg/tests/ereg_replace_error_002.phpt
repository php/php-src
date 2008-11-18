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

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EPAREN in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EBRACK in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EPAREN in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_BADBR in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_BADBR in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_ERANGE in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_EESCAPE in %s on line %d
bool(false)

Deprecated: Function ereg_replace() is deprecated in %s on line %d

Warning: ereg_replace(): REG_ERANGE in %s on line %d
bool(false)
Done
