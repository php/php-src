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

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EPAREN in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EBRACK in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EPAREN in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split() expects parameter 3 to be integer, string given in %s on line %d
NULL

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_BADBR in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_BADBR in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_ERANGE in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split(): REG_EESCAPE in %s on line %d
bool(false)

Deprecated: Function split() is deprecated in %s on line %d

Warning: split() expects parameter 3 to be integer, string given in %s on line %d
NULL
string(8) "original"
Done
