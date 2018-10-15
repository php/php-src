--TEST--
Test array_diff_ukey() function : usage variation - Passing class without string to callback (Handling fatal error)
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments. User supplied function is used for comparing the keys. This function is like array_udiff() but works on the keys instead of the values. The associativity is preserved.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array1 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);
$array2 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array3 = array(1, 2, 3, 4, 5);

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

var_dump( array_diff_ukey($array1, $array2, $value) );
var_dump( array_diff_ukey($array1, $array2, $array3, $value) );

?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : usage variation ***
Error: 2 - array_diff_ukey() expects parameter 3 to be a valid callback, no array or string given, %s(%d)
NULL
Error: 2 - array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given, %s(%d)
NULL
===DONE===
