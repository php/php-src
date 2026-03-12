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
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}

try {
    $ao->uksort(1,2);
} catch (ArgumentCountError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
ArrayObject::uksort() expects exactly 1 argument, 0 given
ArrayObject::uksort() expects exactly 1 argument, 2 given
