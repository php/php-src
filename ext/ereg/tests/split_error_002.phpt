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

$regs = b'original';

var_dump(split(b"", "hello"));
var_dump(split(b"c(d", "hello"));
var_dump(split(b"a[b", "hello"));
var_dump(split(b"c(d", "hello"));
var_dump(split(b"*", "hello"));
var_dump(split(b"+", "hello"));
var_dump(split(b"?", "hello"));
var_dump(split(b"(+?*)", "hello", $regs));
var_dump(split(b"h{256}", "hello"));
var_dump(split(b"h|", "hello"));
var_dump(split(b"h{0}", "hello"));
var_dump(split(b"h{2,1}", "hello"));
var_dump(split(b'[a-c-e]', 'd'));
var_dump(split(b'\\', 'x'));
var_dump(split(b'([9-0])', '1', $regs));

//ensure $regs unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
*** Testing split() : error conditions ***

Warning: split(): REG_EMPTY in %s on line %d
bool(false)

Warning: split(): REG_EPAREN in %s on line %d
bool(false)

Warning: split(): REG_EBRACK in %s on line %d
bool(false)

Warning: split(): REG_EPAREN in %s on line %d
bool(false)

Warning: split(): REG_BADRPT in %s on line %d
bool(false)

Warning: split(): REG_BADRPT in %s on line %d
bool(false)

Warning: split(): REG_BADRPT in %s on line %d
bool(false)

Warning: split() expects parameter 3 to be long, binary string given in %s on line %d
NULL

Warning: split(): REG_BADBR in %s on line %d
bool(false)

Warning: split(): REG_EMPTY in %s on line %d
bool(false)

Warning: split(): REG_EMPTY in %s on line %d
bool(false)

Warning: split(): REG_BADBR in %s on line %d
bool(false)

Warning: split(): REG_ERANGE in %s on line %d
bool(false)

Warning: split(): REG_EESCAPE in %s on line %d
bool(false)

Warning: split() expects parameter 3 to be long, binary string given in %s on line %d
NULL
string(8) "original"
Done
