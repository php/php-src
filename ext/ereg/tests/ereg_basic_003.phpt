--TEST--
Test ereg() function : basic functionality - long RE
--FILE--
<?php
/* Prototype  : proto int ereg(string pattern, string string [, array registers])
 * Description: Regular expression match 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test a long RE with lots of matches
 */

var_dump(ereg(str_repeat('(.)', 2048), str_repeat('x', 2048)));
var_dump(ereg(str_repeat('(.)', 2048), str_repeat('x', 2048), $regs));
var_dump(count($regs));

echo "Done";
?>
--EXPECTF--
Deprecated: Function ereg() is deprecated in %s on line %d
int(1)

Deprecated: Function ereg() is deprecated in %s on line %d
int(2048)
int(2049)
Done
