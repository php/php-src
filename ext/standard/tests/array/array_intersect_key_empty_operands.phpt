--TEST--
array_intersect_key() with empty operands
--FILE--
<?php

echo "Empty operands:\n";
var_dump(array_intersect_key([], ['a' => 1], ['a' => 1]));
var_dump(array_intersect_key(['a' => 1], [], ['a' => 1]));
var_dump(array_intersect_key(['a' => 1], ['a' => 1], []));

echo "Invalid argument after empty operand:\n";
try {
    array_intersect_key(['a' => 1], [], 42);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Single argument:\n";
$single = [3 => 'three', 'a' => 'A', 1 => 'one'];
var_dump(array_intersect_key($single) === $single);

echo "Result order:\n";
$result = array_intersect_key($single, [1 => null, 3 => null, 'a' => null]);
var_dump(array_keys($result));

echo "Append to empty result:\n";
$result = array_intersect_key([9 => 'nine'], []);
$result[] = 'appended';
var_dump(array_keys($result));

echo "Single argument with stale next free index:\n";
$single = [2 => 'two', 100 => 'removed'];
unset($single[100]);
$result = array_intersect_key($single);
$result[] = 'appended';
var_dump(array_keys($result));

?>
--EXPECT--
Empty operands:
array(0) {
}
array(0) {
}
array(0) {
}
Invalid argument after empty operand:
array_intersect_key(): Argument #3 must be of type array, int given
Single argument:
bool(true)
Result order:
array(3) {
  [0]=>
  int(3)
  [1]=>
  string(1) "a"
  [2]=>
  int(1)
}
Append to empty result:
array(1) {
  [0]=>
  int(0)
}
Single argument with stale next free index:
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
