--TEST--
Test array_intersect_uassoc() function : error conditions 
--FILE--
<?php
/* Prototype  : array array_intersect_uassoc(array arr1, array arr2 [, array ...], callback key_compare_func)
 * Description: Computes the intersection of arrays with additional index check, compares indexes by a callback function
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_uassoc() : error conditions ***\n";

// Initialise function arguments 
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");
$array3 = array("a"=>"green", "brown");
$extra_arg = 10;

//Callback function
function key_compare_func($a, $b) {
    if ($a === $b) {
        return 0;
    }
    return ($a > $b) ? 1 : -1;
}

//Test array_intersect_uassoc with one more than the expected number of arguments
echo "\n-- Testing array_intersect_uassoc() function with more than expected no. of arguments --\n";
var_dump( array_intersect_uassoc($array1, $array2, 'key_compare_func',$extra_arg) );

// Testing array_intersect_uassoc with one less than the expected number of arguments
echo "\n-- Testing array_intersect_uassoc() function with less than expected no. of arguments --\n";
var_dump( array_intersect_uassoc($array1, $array2) );
var_dump( array_intersect_uassoc($array1, $array2, $array3) );

// Testing array_intersect_uassoc with no arguments
echo "\n-- Testing array_intersect_uassoc() function with no arguments --\n";
var_dump( array_intersect_uassoc() );
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_uassoc() : error conditions ***

-- Testing array_intersect_uassoc() function with more than expected no. of arguments --

Warning: array_intersect_uassoc() expects parameter 4 to be a valid callback, no array or string given in %s on line %d
NULL

-- Testing array_intersect_uassoc() function with less than expected no. of arguments --

Warning: array_intersect_uassoc(): at least 3 parameters are required, 2 given in %s on line %d
NULL

Warning: array_intersect_uassoc() expects parameter 3 to be a valid callback, second array member is not a valid method in %s on line %d
NULL

-- Testing array_intersect_uassoc() function with no arguments --

Warning: array_intersect_uassoc(): at least 3 parameters are required, 0 given in %s on line %d
NULL
===DONE===
