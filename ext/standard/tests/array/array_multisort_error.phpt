--TEST--
Test array_multisort() function : error conditions
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_multisort() : error conditions ***\n";

echo "\n-- Testing array_multisort() function with repeated flags --\n";
$ar1 = array(1);
try {
    var_dump( array_multisort($ar1, SORT_ASC, SORT_ASC) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_multisort() function with repeated flags --\n";
$ar1 = array(1);
try {
    var_dump( array_multisort($ar1, SORT_STRING, SORT_NUMERIC) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing array_multisort() : error conditions ***

-- Testing array_multisort() function with repeated flags --
array_multisort(): Argument #3 ($sort_flags) must be an array or a sort flag that has not already been specified

-- Testing array_multisort() function with repeated flags --
array_multisort(): Argument #3 ($sort_flags) must be an array or a sort flag that has not already been specified
