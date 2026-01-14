--TEST--
PFA in constexpr 001
--FILE--
<?php

function f($a = g("foo", ?)) {
    return $a(1);
}

function g(string $a, int $b) {
    return [$a, $b];
}

var_dump(f());
var_dump(f());

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  int(1)
}
