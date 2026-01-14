--TEST--
PFA in constexpr: extra named args
--FILE--
<?php

function f($a = g(?, foo: 'bar', ...)) {
    return $a(1, bar: 'baz');
}

function g(...$args) {
    return [$args];
}

var_dump(f());

?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    ["foo"]=>
    string(3) "bar"
    ["bar"]=>
    string(3) "baz"
  }
}
