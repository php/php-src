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

class C {
    static function f($a = self::g("foo", ?)) {
        return $a(1);
    }

    static function g(string $a, int $b) {
        return [$a, $b];
    }
}

var_dump(f());
var_dump(f());
var_dump(C::f());
var_dump(C::f());

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
