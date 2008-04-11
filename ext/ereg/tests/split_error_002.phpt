--TEST--
Test split() function : error conditions - test bad regular expressions
--FILE--
<?php
/* Prototype  : proto array split(string pattern, string string [, int limit])
 * Description: Split string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test bad regular expressions
 */

echo "*** Testing split() : error conditions ***\n";

$regs = 'original';

var_dump(split("", "hello"));
var_dump(split("c(d", "hello"));
var_dump(split("a[b", "hello"));
var_dump(split("c(d", "hello"));
var_dump(split("*", "hello"));
var_dump(split("+", "hello"));
var_dump(split("?", "hello"));
var_dump(split("(+?*)", "hello", $regs));
var_dump(split("h{256}", "hello"));
var_dump(split("h|", "hello"));
var_dump(split("h{0}", "hello"));
var_dump(split("h{2,1}", "hello"));
var_dump(split('[a-c-e]', 'd'));
var_dump(split('\\', 'x'));
var_dump(split('([9-0])', '1', $regs));

//ensure $regs unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
*** Testing split() : error conditions ***

Warning: split(): REG_EMPTY in %s on line 16
bool(false)

Warning: split(): REG_EPAREN in %s on line 17
bool(false)

Warning: split(): REG_EBRACK in %s on line 18
bool(false)

Warning: split(): REG_EPAREN in %s on line 19
bool(false)

Warning: split(): REG_BADRPT in %s on line 20
bool(false)

Warning: split(): REG_BADRPT in %s on line 21
bool(false)

Warning: split(): REG_BADRPT in %s on line 22
bool(false)

Warning: split(): REG_BADRPT in %s on line 23
bool(false)

Warning: split(): REG_BADBR in %s on line 24
bool(false)

Warning: split(): REG_EMPTY in %s on line 25
bool(false)

Warning: split(): REG_EMPTY in %s on line 26
bool(false)

Warning: split(): REG_BADBR in %s on line 27
bool(false)

Warning: split(): REG_ERANGE in %s on line 28
bool(false)

Warning: split(): REG_EESCAPE in %s on line 29
bool(false)

Warning: split(): REG_ERANGE in %s on line 30
bool(false)
string(8) "original"
Done