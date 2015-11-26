--TEST--
Test get_magic_quotes_gpc() function
--FILE--
<?php
/* Prototype: int get_magic_quotes_gpc  ( void  )
 * This function is not supported anymore and will always return false
 */

echo "Simple testcase for get_magic_quotes_gpc() function\n";
var_dump(get_magic_quotes_gpc());

echo "\n-- Error cases --\n"; 
// no checks on number of args
var_dump(get_magic_quotes_gpc(true)); 

?>
===DONE===
--EXPECTF--
Simple testcase for get_magic_quotes_gpc() function
bool(false)

-- Error cases --

Warning: get_magic_quotes_gpc() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
