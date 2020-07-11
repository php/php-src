--TEST--
Test next() function : usage variation - Mulit-dimensional arrays
--FILE--
<?php
/*
 * Test next() when passed:
 * 1. a two-dimensional array
 * 2. a sub-array
 * as $array_arg argument.
 */

echo "*** Testing next() : usage variations ***\n";

$array_arg = array ('a' => 'z', array(9, 8, 7));

echo "\n-- Pass a two-dimensional array as \$array_arg --\n";
var_dump(next($array_arg));
var_dump(next($array_arg));

echo "\n-- Pass a sub-array as \$array_arg --\n";
var_dump(next($array_arg[0]));
?>
--EXPECT--
*** Testing next() : usage variations ***

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
