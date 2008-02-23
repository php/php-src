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

echo "\n-- Pass array by reference --\n";
$new_input = array (1, 2, 3);
var_dump(array_slice(&$new_input, 1));
echo "-- Check passed array: --\n";
var_dump($new_input);

echo "Done";
?>

--EXPECTF--
Deprecated: Call-time pass-by-reference has been deprecated in %s on line %d
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

-- Pass array by reference --
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
-- Check passed array: --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
--UEXPECTF--
Deprecated: Call-time pass-by-reference has been deprecated in %s on line %d
*** Testing array_slice() : usage variations ***

-- Array of referenced variables ($preserve_keys = default) --
array(2) {
  [0]=>
  &unicode(3) "two"
  [1]=>
  &unicode(5) "three"
}
-- Change $val2 ($preserve_keys = TRUE) --
array(2) {
  [2]=>
  &unicode(12) "hello, world"
  [1]=>
  &unicode(5) "three"
}

-- Pass array by reference --
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
-- Check passed array: --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
