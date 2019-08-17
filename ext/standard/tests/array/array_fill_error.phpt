--TEST--
Test array_fill() function : error conditions
--FILE--
<?php
/* Prototype  : proto array array_fill(int start_key, int num, mixed val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val
 * Source code: ext/standard/array.c
*/


echo "*** Testing array_fill() : error conditions ***\n";

// calling array_fill with negative values for 'num' parameter
$start_key = 0;
$num = -1;
$val = 1;

try {
    var_dump( array_fill($start_key,$num,$val) );
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>

DONE
--EXPECTF--
*** Testing array_fill() : error conditions ***
Number of elements can't be negative

DONE
