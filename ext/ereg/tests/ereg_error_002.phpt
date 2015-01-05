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

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EPAREN in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EBRACK in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EPAREN in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_BADRPT in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_BADBR in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EMPTY in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_BADBR in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_ERANGE in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_EESCAPE in %s on line %d
bool(false)

Deprecated: Function ereg() is deprecated in %s on line %d

Warning: ereg(): REG_ERANGE in %s on line %d
bool(false)
string(8) "original"
Done
