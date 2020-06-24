--TEST--
Test ArrayObject::uksort() function : wrong arg count
--FILE--
<?php
/* Sort the entries by key using user defined function.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

$ao = new ArrayObject();

try {
    $ao->uksort();
} catch (BadMethodCallException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $ao->uksort(1,2);
} catch (BadMethodCallException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Function expects exactly one argument
Function expects exactly one argument
