--TEST--
Test array_diff_uassoc() function : error conditions
--FILE--
<?php
/* Prototype  : array array_diff_uassoc(array arr1, array arr2 [, array ...], callback key_comp_func)
 * Description: Computes the difference of arrays with additional index check which is performed by a
 * 				user supplied callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_diff_uassoc() : error conditions ***\n";

//Initialize array
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");
$array3 = array("a" => "green", "red");
$array4 = array();
$extra_arg = array(1, 2, 3, 4);

function key_compare_func($a, $b)
{
    if ($a === $b) {
        return 0;
    }
    return ($a > $b)? 1:-1;
}

//Test array_diff_uassoc with one more than the expected number of arguments
echo "\n-- Testing array_diff_uassoc() function with more than expected no. of arguments --\n";
var_dump( array_diff_uassoc($array1, $array2, "key_compare_func", $extra_arg) );
var_dump( array_diff_uassoc($array1, $array2, $array3, $array4, "key_compare_func", $extra_arg) );

// Testing array_diff_uassoc with one less than the expected number of arguments
echo "\n-- Testing array_diff_uassoc() function with less than expected no. of arguments --\n";
var_dump( array_diff_uassoc($array1, $array2) );

// Testing array_diff_uassoc with no arguments
echo "\n-- Testing array_diff_uassoc() function with no arguments --\n";
var_dump( array_diff_uassoc() );
?>
===DONE===
--EXPECTF--
*** Testing array_diff_uassoc() : error conditions ***

-- Testing array_diff_uassoc() function with more than expected no. of arguments --

Warning: array_diff_uassoc() expects parameter 4 to be a valid callback, array must have exactly two members in %s on line %d
NULL

Warning: array_diff_uassoc() expects parameter 6 to be a valid callback, array must have exactly two members in %s on line %d
NULL

-- Testing array_diff_uassoc() function with less than expected no. of arguments --

Warning: array_diff_uassoc(): at least 3 parameters are required, 2 given in %s on line %d
NULL

-- Testing array_diff_uassoc() function with no arguments --

Warning: array_diff_uassoc(): at least 3 parameters are required, 0 given in %s on line %d
NULL
===DONE===
