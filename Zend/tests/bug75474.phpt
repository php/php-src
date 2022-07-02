--TEST--
Bug #75474: function scope static variables are not bound to a unique function
--FILE--
<?php

function bar($k, $v) {
    static $foo = [];
    $foo[$k] = $v;
    return $foo;
}

var_dump(bar(0, 0));
var_dump(Closure::fromCallable("bar")(1, 1));
var_dump(bar(2, 2));
var_dump(Closure::fromCallable("bar")(3, 3));
$RF = new ReflectionFunction("bar");
var_dump($RF->getClosure()(4, 4));
var_dump(bar(5, 5));

?>
--EXPECT--
array(1) {
  [0]=>
  int(0)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
