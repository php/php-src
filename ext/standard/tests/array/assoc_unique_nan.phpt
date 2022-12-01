--TEST--
Assoc\unique() preserves NAN
--FILE--
<?php

var_dump(Assoc\unique([
    NAN,
    NAN,
]));

var_dump(Assoc\unique([
    'foo' => NAN,
    'bar' => NAN,
]));

var_dump(Assoc\unique([
    [NAN],
    1.0,
    [NAN],
]));

var_dump(Assoc\unique([
    1.0,
    NAN,
    1.0,
    NAN,
    1.0,
]));

var_dump(Assoc\unique([
    1.0,
    NAN,
    1.0,
]));

?>
--EXPECT--
array(2) {
  [0]=>
  float(NAN)
  [1]=>
  float(NAN)
}
array(2) {
  ["foo"]=>
  float(NAN)
  ["bar"]=>
  float(NAN)
}
array(3) {
  [0]=>
  array(1) {
    [0]=>
    float(NAN)
  }
  [1]=>
  float(1)
  [2]=>
  array(1) {
    [0]=>
    float(NAN)
  }
}
array(3) {
  [0]=>
  float(1)
  [1]=>
  float(NAN)
  [3]=>
  float(NAN)
}
array(2) {
  [0]=>
  float(1)
  [1]=>
  float(NAN)
}
