--TEST--
Test array_diff_ukey() function : error conditions
--FILE--
<?php
/* Prototype  : array array_diff_ukey(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Returns the entries of arr1 that have keys which are not present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_ukey() : error conditions ***\n";

// Initialize
$array1 = array('blue'  => 1, 'red'  => 2, 'green'  => 3, 'purple' => 4);
$array2 = array('green' => 5, 'blue' => 6, 'yellow' => 7, 'cyan'   => 8);
$extra_arg = 10;

function key_compare_func($key1, $key2)
{
    if ($key1 == $key2) {
        return 0;
    }
    return ($key1 > $key2)? 1:-1;
}

//Test array_diff_ukey with one more than the expected number of arguments
echo "\n-- Testing array_diff_ukey() function with more than expected no. of arguments --\n";
var_dump( array_diff_ukey($array1, $array2, 'key_compare_func', $extra_arg) );

// Testing array_diff_ukey with one less than the expected number of arguments
echo "\n-- Testing array_diff_ukey() function with less than expected no. of arguments --\n";
var_dump( array_diff_ukey($array1, $array2) );

// Testing array_diff_ukey with one less than the expected number of arguments
echo "\n-- Testing array_diff_ukey() function with no arguments --\n";
var_dump( array_diff_ukey() );
?>
===DONE===
--EXPECTF--
*** Testing array_diff_ukey() : error conditions ***

-- Testing array_diff_ukey() function with more than expected no. of arguments --

Warning: array_diff_ukey() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

-- Testing array_diff_ukey() function with less than expected no. of arguments --

Warning: array_diff_ukey(): at least 3 parameters are required, 2 given in %s on line %d
NULL

-- Testing array_diff_ukey() function with no arguments --

Warning: array_diff_ukey(): at least 3 parameters are required, 0 given in %s on line %d
NULL
===DONE===
