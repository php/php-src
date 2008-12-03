--TEST--
Test ereg() function : error conditions - test bad regular expressions
--FILE--
<?php
/* Prototype  : proto int ereg(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test bad regular expressions
 */

echo "*** Testing ereg() : error conditions ***\n";

$regs = 'original';

var_dump(ereg("", "hello"));
var_dump(ereg("c(d", "hello"));
var_dump(ereg("a[b", "hello"));
var_dump(ereg("c(d", "hello"));
var_dump(ereg("*", "hello"));
var_dump(ereg("+", "hello"));
var_dump(ereg("?", "hello"));
var_dump(ereg("(+?*)", "hello", $regs));
var_dump(ereg("h{256}", "hello"));
var_dump(ereg("h|", "hello"));
var_dump(ereg("h{0}", "hello"));
var_dump(ereg("h{2,1}", "hello"));
var_dump(ereg('[a-c-e]', 'd'));
var_dump(ereg('\\', 'x'));
var_dump(ereg('([9-0])', '1', $regs));

//ensure $regs unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
*** Testing ereg() : error conditions ***

Warning: ereg(): REG_EMPTY in %s on line 16
bool(false)

Warning: ereg(): REG_EPAREN in %s on line 17
bool(false)

Warning: ereg(): REG_EBRACK in %s on line 18
bool(false)

Warning: ereg(): REG_EPAREN in %s on line 19
bool(false)

Warning: ereg(): REG_BADRPT in %s on line 20
bool(false)

Warning: ereg(): REG_BADRPT in %s on line 21
bool(false)

Warning: ereg(): REG_BADRPT in %s on line 22
bool(false)

Warning: ereg(): REG_BADRPT in %s on line 23
bool(false)

Warning: ereg(): REG_BADBR in %s on line 24
bool(false)

Warning: ereg(): REG_EMPTY in %s on line 25
bool(false)

Warning: ereg(): REG_EMPTY in %s on line 26
bool(false)

Warning: ereg(): REG_BADBR in %s on line 27
bool(false)

Warning: ereg(): REG_ERANGE in %s on line 28
bool(false)

Warning: ereg(): REG_EESCAPE in %s on line 29
bool(false)

Warning: ereg(): REG_ERANGE in %s on line 30
bool(false)
string(8) "original"
Done