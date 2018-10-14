--TEST--
Test get_magic_quotes_runtime() function
--FILE--
<?php
/* Prototype: int get_magic_quotes_runtime  ( void  )
 * This function is not supported anymore and will always return false
 */

echo "Simple testcase for get_magic_quotes_runtime() function\n";

var_dump(get_magic_quotes_runtime());

echo "\n-- Error cases --\n";
// no checks on number of args
var_dump(get_magic_quotes_runtime(true));

?>
===DONE===
--EXPECTF--
Simple testcase for get_magic_quotes_runtime() function
bool(false)

-- Error cases --

Warning: get_magic_quotes_runtime() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
