--TEST--
Vector constructed from array
--FILE--
<?php
// discards keys
$vec = new Vector(['first' => 'x', 'second' => new stdClass()]);
foreach ($vec as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($vec);
var_dump((array)$vec);

$vec = new Vector([]);
var_dump($vec);
var_dump((array)$vec);
foreach ($vec as $key => $value) {
    echo "Unreachable\n";
}

// Vector will always reindex keys in the order of iteration, like array_values() does.
$vec = new Vector([2 => 'a', 0 => 'b']);
var_dump($vec);

var_dump(new Vector([-1 => new stdClass()]));
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
object(Vector)#1 (2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
object(Vector)#3 (1) {
  [0]=>
  object(stdClass)#4 (0) {
  }
}