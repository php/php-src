--TEST--
Test var_dump() function with arrays
--INI--
precision=14
--FILE--
<?php

$values = [
  [],
  [null],
  [false],
  [true],
  [''],
  [[], []],
  [[1, 2], ['a', 'b']],
  [1 => 'One'],
  ["test" => "is_array"],
  [0],
  [-1],
  [10.5, 5.6],
  ['string', 'test'],
];

foreach ($values as $value) {
    var_dump($value);
}

?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  bool(false)
}
array(1) {
  [0]=>
  bool(true)
}
array(1) {
  [0]=>
  string(0) ""
}
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "b"
  }
}
array(1) {
  [1]=>
  string(3) "One"
}
array(1) {
  ["test"]=>
  string(8) "is_array"
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(-1)
}
array(2) {
  [0]=>
  float(10.5)
  [1]=>
  float(5.6)
}
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  string(4) "test"
}
