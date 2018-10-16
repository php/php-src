--TEST--
Test count() function : basic functionality
--FILE--
<?php
/* Prototype  : int count(mixed $var [, int $mode])
 * Description: Count the number of elements in a variable (usually an array)
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of count() using an array as $var argument
 * and different values as $mode argument.
 */

echo "*** Testing count() : basic functionality ***\n";

echo "\n-- One Dimensional Array: --\n";
$array = array('zero', 'one', 'two');
var_dump(count($array));

echo "\n-- Two Dimensional Array: --\n";
$array_multi = array('zero', array(1, 2, 3), 'two');
echo "\$mode = COUNT_NORMAL:    ";
var_dump(count($array_multi, COUNT_NORMAL));
echo "\$mode = 0:               ";
var_dump(count($array_multi, 0));
echo "\$mode = COUNT_RECURSIVE: ";
var_dump(count($array_multi, COUNT_RECURSIVE));
echo "\$mode = 1:               ";
var_dump(count($array_multi, 1));

echo "Done";
?>
--EXPECT--
*** Testing count() : basic functionality ***

-- One Dimensional Array: --
int(3)

-- Two Dimensional Array: --
$mode = COUNT_NORMAL:    int(3)
$mode = 0:               int(3)
$mode = COUNT_RECURSIVE: int(6)
$mode = 1:               int(6)
Done
