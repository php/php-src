--TEST--
array_uintersect_assoc() with empty operands
--FILE--
<?php

$callbackCount = 0;
$compare = static function (mixed $left, mixed $right) use (&$callbackCount): int {
    $callbackCount++;
    return $left <=> $right;
};

echo "Single operand:\n";
var_dump(array_uintersect_assoc(['k' => 1], $compare));
var_dump($callbackCount);
var_dump(array_uintersect_assoc([], $compare));
var_dump($callbackCount);

echo "Second empty operand:\n";
$result = array_uintersect_assoc(['k' => 1], [], ['k' => 1], $compare);
var_dump($result);
var_dump($callbackCount);

echo "Third empty operand:\n";
$callbackCount = 0;
$result = array_uintersect_assoc(['k' => 1], ['k' => 1], [], $compare);
var_dump($result);
var_dump($callbackCount);

echo "Invalid callback with empty operands:\n";
try {
    array_uintersect_assoc([], [], '__missing_array_intersect_callback__');
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Invalid argument after empty operand:\n";
try {
    array_uintersect_assoc(['k' => 1], [], 42, $compare);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Single operand:
array(1) {
  ["k"]=>
  int(1)
}
int(0)
array(0) {
}
int(0)
Second empty operand:
array(0) {
}
int(0)
Third empty operand:
array(0) {
}
int(1)
Invalid callback with empty operands:
array_uintersect_assoc(): Argument #3 must be a valid callback, function "__missing_array_intersect_callback__" not found or invalid function name
Invalid argument after empty operand:
array_uintersect_assoc(): Argument #3 must be of type array, int given
