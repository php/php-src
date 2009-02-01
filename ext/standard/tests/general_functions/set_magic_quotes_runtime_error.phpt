--TEST--
Test set_magic_quotes_runtime() function - error conditions - pass function incorrect arguments
--FILE--
<?php
/* Prototype: bool set_magic_quotes_runtime  ( int $new_setting  )
 * Description: Sets the current active configuration setting of magic_quotes_runtime
*/

echo "Simple testcase for set_magic_quotes_runtime()  - error test\n";

//Note: No error msgs on invalid input; just a return value of FALSE

echo "\n-- Testing is_callable() function with less than expected no. of arguments --\n";
var_dump(set_magic_quotes_runtime());

echo "\n-- Testing is_callable() function with more than expected no. of arguments --\n";
var_dump(set_magic_quotes_runtime(1, true));

?>
===DONE===
--EXPECT--
Simple testcase for set_magic_quotes_runtime()  - error test

-- Testing is_callable() function with less than expected no. of arguments --
bool(false)

-- Testing is_callable() function with more than expected no. of arguments --
bool(false)
===DONE===