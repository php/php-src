--TEST--
PFA in constexpr: named args
--FILE--
<?php

function f($a = g(c: ?, ...)) {
    return $a(1.5, b: 3);
}

function g(string $a = 'hello', int $b = 2, float $c = 0) {
    return [$a, $b, $c];
}

var_dump(f());

?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "hello"
  [1]=>
  int(3)
  [2]=>
  float(1.5)
}
