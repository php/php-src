--TEST--
Test array_diff_uassoc() function : usage variation -Passing classWithoutToString (handling fatal error) to callback
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a 
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : usage variation ***\n";

//Initialize array
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");

class classWithoutToString
{
}

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

$value = new classWithoutToString();
var_dump( array_diff_uassoc($array1, $array2, $value) );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_uassoc() : usage variation ***
Error: 2 - array_diff_uassoc() expects parameter 3 to be a valid callback, no array or string given, %s(%d)
NULL
===DONE===

