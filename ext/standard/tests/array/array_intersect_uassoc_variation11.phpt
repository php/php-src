--TEST--
Test array_intersect_uassoc() function : usage variation - Passing class without string to callback (Handling fatal error)
--SKIPIF--
<?php
$php_version = phpversion();
if(stristr($php_version, "5.2" ) == FALSE){
  die('skip Test is applicable only for PHP5.2');
}
?>
--FILE--
<?php
/* Prototype  : array array_intersect_uassoc(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays with additional index check, compares indexes by a callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

// Initialise function arguments 
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");
$array3 = array("a"=>"green", "brown");
// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
        if (error_reporting() != 0) {
                // report non-silenced errors
                echo "Error: $err_no - $err_msg, $filename($linenum)\n";
        }
}
set_error_handler('test_error_handler');


class classWithoutToString
{
}

$value = new classWithoutToString();

var_dump( array_intersect_uassoc($array1, $array2, $value) );
var_dump( array_intersect_uassoc($array1, $array2, $array3, $value) );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_uassoc() : usage variation ***
Error: 4096 - Object of class classWithoutToString could not be converted to string, %s(%d)
Error: 2 - array_intersect_uassoc(): Not a valid callback , %s(%d)
NULL
Error: 4096 - Object of class classWithoutToString could not be converted to string, %s(%d)
Error: 2 - array_intersect_uassoc(): Not a valid callback , %s(%d)
NULL
===DONE===
