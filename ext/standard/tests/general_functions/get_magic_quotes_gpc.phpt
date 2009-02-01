--TEST--
Test get_magic_quotes_gpc() function
--FILE--
<?php
/* Prototype: int get_magic_quotes_gpc  ( void  )
 * Description: Gets the current configuration setting of magic quotes gpc
 * 
 * On PHP 6 this function always returns FALSE
*/

echo "Simple testcase for get_magic_quotes_gpc() function\n";

var_dump(get_magic_quotes_gpc()); 

echo "\n-- Error cases --\n"; 
// no checks on number of args
var_dump(get_magic_quotes_gpc(true)); 

?>
===DONE===
--EXPECT--
Simple testcase for get_magic_quotes_gpc() function
bool(false)

-- Error cases --
bool(false)
===DONE===