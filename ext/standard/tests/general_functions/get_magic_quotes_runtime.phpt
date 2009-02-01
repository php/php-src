--TEST--
Test get_magic_quotes_runtime() function
--FILE--
<?php
/* Prototype: int get_magic_quotes_runtime  ( void  )
 * Description: Gets the current active configuration setting of magic_quotes_runtime
 *
 * On PHP 6 this function always returns FALSE
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
bool(false)
===DONE===