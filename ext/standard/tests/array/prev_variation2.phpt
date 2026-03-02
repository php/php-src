--TEST--
Test prev() function : usage variation - Multi-dimensional arrays
--FILE--
<?php
/*
 * Test prev() when passed:
 * 1. a two-dimensional array
 * 2. a sub-array
 * as $array_arg argument.
 */

echo "*** Testing prev() : usage variations ***\n";

$subarray = array(9,8,7);
end($subarray);

$array_arg = array ($subarray, 'a' => 'z');
end($array_arg);

echo "\n-- Pass a two-dimensional array as \$array_arg --\n";
var_dump(prev($array_arg));
var_dump(prev($array_arg));

echo "\n-- Pass a sub-array as \$array_arg --\n";
var_dump(prev($array_arg[0]));
?>
--EXPECT--
*** Testing prev() : usage variations ***

-- Pass a two-dimensional array as $array_arg --
array(3) {
  [0]=>
  int(9)
  [1]=>
  int(8)
  [2]=>
  int(7)
}
bool(false)

-- Pass a sub-array as $array_arg --
int(8)
