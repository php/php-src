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

function twoArgs($v, $w) {
  return $v + $w;
}

function fourArgs($v, $w, $x, $y) {
  return $v + $w + $x + $y;
}

$array = array(1);

echo "\n--- Testing with a callback with one parameter ---\n";
var_dump(array_reduce($array, "oneArg", 2));

echo "\n--- Testing with a callback with two parameters ---\n";
var_dump(array_reduce($array, "twoArgs", 2));

echo "\n--- Testing with a callback with too many parameters ---\n";
var_dump(array_reduce($array, "fourArgs", 2));

?>
===DONE===
--EXPECTF--
*** Testing array_reduce() : variation ***

--- Testing with a callback with one parameter ---
int(2)

--- Testing with a callback with two parameters ---
int(3)

--- Testing with a callback with too many parameters ---

Warning: Missing argument 4 for fourArgs() in %sarray_reduce_variation1.php on line %d

Notice: Undefined variable: y in %sarray_reduce_variation1.php on line %d
int(3)
===DONE===