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
} catch (BadMethodCallException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $ao->uasort(1,2);
} catch (BadMethodCallException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Function expects exactly one argument
Function expects exactly one argument
