--TEST--
Test array_intersect_ukey() function : usage variation - Passing class without string to callback (Handling fatal error)
--SKIPIF--
<?php
$php_version = phpversion();
if(stristr($php_version, "5.2" ) == FALSE){
  die('skip Test is applicable only for PHP5.2');
}
?>
--FILE--
<?php
/* Prototype  : array array_intersect_ukey(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays using a callback function on the keys for comparison. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_uassoc() : usage variation ***\n";

//Initialise arguments
$array1 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);
$array3 = array("a"=>"green", "cyan");

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

var_dump( array_intersect_ukey($array1, $array2, $value) );
var_dump( array_intersect_ukey($array1, $array2, $array3, $value) );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_uassoc() : usage variation ***
Error: 4096 - Object of class classWithoutToString could not be converted to string, %s(%d)
Error: 2 - array_intersect_ukey(): Not a valid callback , %s(%d)
NULL
Error: 4096 - Object of class classWithoutToString could not be converted to string, %s(%d)
Error: 2 - array_intersect_ukey(): Not a valid callback , %s(%d)
NULL
===DONE===