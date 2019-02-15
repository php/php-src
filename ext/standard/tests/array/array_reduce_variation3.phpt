--TEST--
Test array_reduce() function : variation - object callbacks
--FILE--
<?php
/* Prototype  : mixed array_reduce(array input, mixed callback [, int initial])
 * Description: Iteratively reduce the array to a single value via the callback.
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

echo "*** Testing array_reduce() : variation - object callbacks ***\n";

class A {
  static function adder($a, $b) {return $a + $b;}
  public function adder2($a, $b) {return $a + $b;}
}

$array = array(1);

echo "\n--- Static method callback ---\n";
var_dump(array_reduce($array, array("A", "adder")));

echo "\n--- Instance method callback ---\n";
var_dump(array_reduce($array, array(new A(), "adder2")));

?>
===DONE===
--EXPECT--
*** Testing array_reduce() : variation - object callbacks ***

--- Static method callback ---
int(1)

--- Instance method callback ---
int(1)
===DONE===
