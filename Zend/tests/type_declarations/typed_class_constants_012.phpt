--TEST--
Typed class constants (declaration; iterable)
--FILE--
<?php
class A {
    public const iterable A = [1];
}

var_dump(A::A);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
