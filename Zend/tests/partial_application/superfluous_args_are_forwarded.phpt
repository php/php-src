--TEST--
PFAs forwards superfluous args iff a variadic placeholder is specified
--FILE--
<?php

function f($a) {
    var_dump(func_get_args());
}

$f = f(?, ...);
$f(1, 2, 3);

$h = f(?);
$h(1, 2, 3);

function g($a,  ...$args) {
    var_dump(func_get_args());
}

$g = g(?, ...);
$g(1, 2, 3);

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(1) {
  [0]=>
  int(1)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
