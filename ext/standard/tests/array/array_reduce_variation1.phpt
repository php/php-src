--TEST--
Test array_reduce() function : variation 
--FILE--
<?php
/* Prototype  : mixed array_reduce(array input, mixed callback [, int initial])
 * Description: Iteratively reduce the array to a single value via the callback. 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_reduce() : variation ***\n";


function oneArg($v) {
  return $v;
}

function threeArgs($v, $w, $x) {
  return $v + $w + $x;
}

$array = array(1);

echo "\n--- Testing with a callback with too few parameters ---\n";
var_dump(array_reduce($array, "oneArg", 2));

echo "\n--- Testing with a callback with too many parameters ---\n";
var_dump(array_reduce($array, "threeArgs", 2));

?>
===DONE===
--EXPECTF--
*** Testing array_reduce() : variation ***

--- Testing with a callback with too few parameters ---
int(2)

--- Testing with a callback with too many parameters ---

Warning: Missing argument 3 for threeArgs() in %sarray_reduce_variation1.php on line %d

Notice: Undefined variable: x in %sarray_reduce_variation1.php on line %d
int(3)
===DONE===