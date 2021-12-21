--TEST--
array_unique() with ARRAY_UNIQUE_IDENTICAL preserves NAN
--FILE--
<?php

var_dump(array_unique([
    NAN,
    NAN,
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    'foo' => NAN,
    'bar' => NAN,
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    [NAN],
    1.0,
    [NAN],
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    1.0,
    NAN,
    1.0,
    NAN,
    1.0,
], ARRAY_UNIQUE_IDENTICAL));

var_dump(array_unique([
    1.0,
    NAN,
    1.0,
], ARRAY_UNIQUE_IDENTICAL));

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
