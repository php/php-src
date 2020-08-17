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
ArrayObject::uasort(): Exactly 1 argument is expected, 0 given
ArrayObject::uasort(): Exactly 1 argument is expected, 2 given
