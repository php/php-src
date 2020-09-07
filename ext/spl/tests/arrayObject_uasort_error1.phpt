--TEST--
Test ArrayObject::uasort() function : wrong arg count
--FILE--
<?php
/* Sort the entries by values user defined function.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

$ao = new ArrayObject();

try {
    $ao->uasort();
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}

try {
    $ao->uasort(1,2);
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
ArrayObject::uasort() expects exactly 1 argument, 0 given
ArrayObject::uasort() expects exactly 1 argument, 2 given
