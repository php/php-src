--TEST--
Vector constructed from array
--FILE--
<?php
// discards keys
$it = new Vector(['first' => 'x', 'second' => new stdClass()], preserveKeys: false);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
var_dump((array)$it);

$it = new Vector([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

// The default is to preserve keys
$it = new Vector([2 => 'third', 0 => 'first']);
var_dump($it);

try {
    $it = new Vector([-1 => new stdClass()]);
} catch (UnexpectedValueException $e) {
    echo "Caught: {$e->getMessage()}\n";
}

try {
    $it = new Vector(['0a' => new stdClass()]);
} catch (UnexpectedValueException $e) {
    echo "Caught: {$e->getMessage()}\n";
}
?>
--EXPECT--
Key: 0
Value: 'x'
Key: 1
Value: (object) array(
)
object(Vector)#1 (2) {
  [0]=>
  string(1) "x"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
object(Vector)#3 (0) {
}
array(0) {
}
object(Vector)#1 (3) {
  [0]=>
  string(5) "first"
  [1]=>
  NULL
  [2]=>
  string(5) "third"
}
Caught: array must contain only positive integer keys
Caught: array must contain only positive integer keys