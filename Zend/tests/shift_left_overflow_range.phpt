--TEST--
Too narrow range inferred for SL when the shift wraps
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--FILE--
<?php
/* $i is inferred as RANGE[1..5]. Shifting it by 62 wraps for every $i >= 2,
 * so the result range may not be derived from the two endpoints: both of them
 * happen to yield 1 << 62, while the values in between do not. */
function shift_positive(): array {
    $out = [];
    for ($i = 1; $i <= 5; $i++) {
        $out[] = $i << 62;
    }
    return $out;
}

/* Likewise for RANGE[-3..-1] shifted by 63: both endpoints yield LONG_MIN,
 * but -2 << 63 is 0. */
function shift_negative(): array {
    $out = [];
    for ($i = -1; $i >= -3; $i--) {
        $out[] = $i << 63;
    }
    return $out;
}

var_dump(shift_positive());
var_dump(shift_negative());
?>
--EXPECT--
array(5) {
  [0]=>
  int(4611686018427387904)
  [1]=>
  int(-9223372036854775808)
  [2]=>
  int(-4611686018427387904)
  [3]=>
  int(0)
  [4]=>
  int(4611686018427387904)
}
array(3) {
  [0]=>
  int(-9223372036854775808)
  [1]=>
  int(0)
  [2]=>
  int(-9223372036854775808)
}
