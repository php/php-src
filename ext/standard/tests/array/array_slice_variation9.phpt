--TEST--
Test array_slice() function : usage variations - referenced variables
--FILE--
<?php
/* Prototype  : array array_slice(array $input, int $offset [, int $length [, bool $preserve_keys]])
 * Description: Returns elements specified by offset and length 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_slice() when:
 * 1. Passed an array of referenced variables
 * 2. $input argument is passed by reference
 */

echo "*** Testing array_slice() : usage variations ***\n";

$val1 = 'one';
$val2 = 'two';
$val3 = 'three';

echo "\n-- Array of referenced variables (\$preserve_keys = default) --\n";
$input = array(3 => &$val1, 2 => &$val2, 1 => &$val3);
var_dump(array_slice($input, 1, 2));

echo "-- Change \$val2 (\$preserve_keys = TRUE) --\n";
$val2 = 'hello, world';
var_dump(array_slice($input, 1, 2, true));

echo "Done";
?>

--EXPECTF--
*** Testing array_slice() : usage variations ***

-- Array of referenced variables ($preserve_keys = default) --
array(2) {
  [0]=>
  &string(3) "two"
  [1]=>
  &string(5) "three"
}
-- Change $val2 ($preserve_keys = TRUE) --
array(2) {
  [2]=>
  &string(12) "hello, world"
  [1]=>
  &string(5) "three"
}
Done
