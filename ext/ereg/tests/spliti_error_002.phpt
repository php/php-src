--TEST--
Test spliti() function : error conditions - test bad regular expressions
--FILE--
<?php
/* Prototype  : proto array spliti(string pattern, string string [, int limit])
 * Description: spliti string into array by regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test bad regular expressions
 */

echo "*** Testing spliti() : error conditions ***\n";

$regs = 'original';

var_dump(spliti("", "hello"));
var_dump(spliti("c(d", "hello"));
var_dump(spliti("a[b", "hello"));
var_dump(spliti("c(d", "hello"));
var_dump(spliti("*", "hello"));
var_dump(spliti("+", "hello"));
var_dump(spliti("?", "hello"));
var_dump(spliti("(+?*)", "hello", $regs));
var_dump(spliti("h{256}", "hello"));
var_dump(spliti("h|", "hello"));
var_dump(spliti("h{0}", "hello"));
var_dump(spliti("h{2,1}", "hello"));
var_dump(spliti('[a-c-e]', 'd'));
var_dump(spliti('\\', 'x'));
var_dump(spliti('([9-0])', '1', $regs));

//ensure $regs unchanged
var_dump($regs);

echo "Done";
?>
--EXPECTF--
*** Testing spliti() : error conditions ***

Warning: spliti(): REG_EMPTY in %s on line 16
bool(false)

Warning: spliti(): REG_EPAREN in %s on line 17
bool(false)

Warning: spliti(): REG_EBRACK in %s on line 18
bool(false)

Warning: spliti(): REG_EPAREN in %s on line 19
bool(false)

Warning: spliti(): REG_BADRPT in %s on line 20
bool(false)

Warning: spliti(): REG_BADRPT in %s on line 21
bool(false)

Warning: spliti(): REG_BADRPT in %s on line 22
bool(false)

Warning: spliti(): REG_BADRPT in %s on line 23
bool(false)

Warning: spliti(): REG_BADBR in %s on line 24
bool(false)

Warning: spliti(): REG_EMPTY in %s on line 25
bool(false)

Warning: spliti(): REG_EMPTY in %s on line 26
bool(false)

Warning: spliti(): REG_BADBR in %s on line 27
bool(false)

Warning: spliti(): REG_ERANGE in %s on line 28
bool(false)

Warning: spliti(): REG_EESCAPE in %s on line 29
bool(false)

Warning: spliti(): REG_ERANGE in %s on line 30
bool(false)
string(8) "original"
Done