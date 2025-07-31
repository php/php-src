--TEST--
Testing func_get_named_args() with anonymous functions and closures
--FILE--
<?php

$anonymousFunc = function($a, $b = 'default_b', $c = 'default_c') {
    return func_get_named_args();
};

var_dump($anonymousFunc('A', 'B', 'C'));
var_dump($anonymousFunc(c: 'C', a: 'A'));
var_dump($anonymousFunc('A', c: 'C'));

$capturedVar = 'captured';
$closure = function($x, $y = 'default_y') use ($capturedVar) {
    return func_get_named_args();
};

var_dump($closure('X', 'Y'));
var_dump($closure(y: 'Y', x: 'X'));

$variadicFunc = function($first, ...$rest) {
    return func_get_named_args();
};

var_dump($variadicFunc('FIRST', 'extra1', 'extra2'));
var_dump($variadicFunc(first: 'FIRST', extra: 'EXTRA'));

$arrow = fn($p, $q = 'default_q') => func_get_named_args();

var_dump($arrow('P', 'Q'));
var_dump($arrow(q: 'Q', p: 'P'));

$mapped = array_map(function($item, $prefix = 'prefix') {
    return func_get_named_args();
}, ['test']);
var_dump($mapped[0]);

?>
--EXPECT--
array(3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(1) "B"
  ["c"]=>
  string(1) "C"
}
array(3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(9) "default_b"
  ["c"]=>
  string(1) "C"
}
array(3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(9) "default_b"
  ["c"]=>
  string(1) "C"
}
array(2) {
  ["x"]=>
  string(1) "X"
  ["y"]=>
  string(1) "Y"
}
array(2) {
  ["x"]=>
  string(1) "X"
  ["y"]=>
  string(1) "Y"
}
array(3) {
  ["first"]=>
  string(5) "FIRST"
  [1]=>
  string(6) "extra1"
  [2]=>
  string(6) "extra2"
}
array(2) {
  ["first"]=>
  string(5) "FIRST"
  ["extra"]=>
  string(5) "EXTRA"
}
array(2) {
  ["p"]=>
  string(1) "P"
  ["q"]=>
  string(1) "Q"
}
array(2) {
  ["p"]=>
  string(1) "P"
  ["q"]=>
  string(1) "Q"
}
array(1) {
  ["item"]=>
  string(4) "test"
}
