--TEST--
Test eregi() function : error conditions - test bad regular expressions
--FILE--
<?php
/* Prototype  : proto int eregi(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test bad regular expressions
 */

echo "*** Testing eregi() : error conditions ***\n";

$regs = 'original';

var_dump(eregi("", "hello"));
var_dump(eregi("c(d", "hello"));
var_dump(eregi("a[b", "hello"));
var_dump(eregi("c(d", "hello"));
var_dump(eregi("*", "hello"));
var_dump(eregi("+", "hello"));
var_dump(eregi("?", "hello"));
var_dump(eregi("(+?*)", "hello", $regs));
var_dump(eregi("h{256}", "hello"));
var_dump(eregi("h|", "hello"));
var_dump(eregi("h{0}", "hello"));
var_dump(eregi("h{2,1}", "hello"));
var_dump(eregi('[a-c-e]', 'd'));
var_dump(eregi('\\', 'x'));
var_dump(eregi('([9-0])', '1', $regs));

//ensure $regs unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
*** Testing eregi() : error conditions ***

Warning: eregi(): REG_EMPTY in %s on line 16
bool(false)

Warning: eregi(): REG_EPAREN in %s on line 17
bool(false)

Warning: eregi(): REG_EBRACK in %s on line 18
bool(false)

Warning: eregi(): REG_EPAREN in %s on line 19
bool(false)

Warning: eregi(): REG_BADRPT in %s on line 20
bool(false)

Warning: eregi(): REG_BADRPT in %s on line 21
bool(false)

Warning: eregi(): REG_BADRPT in %s on line 22
bool(false)

Warning: eregi(): REG_BADRPT in %s on line 23
bool(false)

Warning: eregi(): REG_BADBR in %s on line 24
bool(false)

Warning: eregi(): REG_EMPTY in %s on line 25
bool(false)

Warning: eregi(): REG_EMPTY in %s on line 26
bool(false)

Warning: eregi(): REG_BADBR in %s on line 27
bool(false)

Warning: eregi(): REG_ERANGE in %s on line 28
bool(false)

Warning: eregi(): REG_EESCAPE in %s on line 29
bool(false)

Warning: eregi(): REG_ERANGE in %s on line 30
bool(false)
string(8) "original"
Done