--TEST--
Test set_magic_quotes_runtime() function - error conditions - pass function incorrect arguments
--FILE--
<?php
/* Prototype: bool set_magic_quotes_runtime  ( int $new_setting  )
 * Description: Sets the current active configuration setting of magic_quotes_runtime
*/

echo "Simple testcase for set_magic_quotes_runtime()  - error test\n";

//Note: No error msgs on invalid input; just a return value of FALSE

echo "\n-- Testing set_magic_quotes_runtime() function with less than expected no. of arguments --\n";
var_dump(set_magic_quotes_runtime());

echo "\n-- Testing set_magic_quotes_runtime() function with more than expected no. of arguments --\n";
var_dump(set_magic_quotes_runtime(1, true));

?>
===DONE===
--EXPECTF--
Simple testcase for set_magic_quotes_runtime()  - error test

-- Testing set_magic_quotes_runtime() function with less than expected no. of arguments --

Deprecated: Function set_magic_quotes_runtime() is deprecated in %s on line %d

Warning: set_magic_quotes_runtime() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing set_magic_quotes_runtime() function with more than expected no. of arguments --

Deprecated: Function set_magic_quotes_runtime() is deprecated in %s on line %d

Warning: set_magic_quotes_runtime() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
