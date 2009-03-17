--TEST--
Test set_magic_quotes_runtime() function -  basic test 
--INI--
magic_quotes_runtime = 0
--FILE--
<?php
/* Prototype: bool set_magic_quotes_runtime  ( int $new_setting  )
 * Description: Sets the current active configuration setting of magic_quotes_runtime
 *
 * On PHP 6 this fucntion is no longer supported
*/

echo "Simple testcase for set_magic_quotes_runtime() function - basic test\n";

echo "\n-- Set magic quotes runtime to 1:  --\n";
var_dump(set_magic_quotes_runtime(1));

?>
--EXPECTF--
Simple testcase for set_magic_quotes_runtime() function - basic test

-- Set magic quotes runtime to 1:  --

Fatal error: Call to undefined function set_magic_quotes_runtime() in %s on line %d