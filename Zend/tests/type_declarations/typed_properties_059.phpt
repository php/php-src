--TEST--
Nullable typed properties in traits
--FILE--
<?php
trait T {
    public int $a1;
    public ?int $b1;
}

class A {
    use T;
    public int $a2;
    public ?int $b2;
}

$x = new A;
var_dump($x);
?>
--EXPECT--
object(A)#1 (0) {
  ["a2"]=>
  uninitialized(int)
  ["b2"]=>
  uninitialized(?int)
  ["a1"]=>
  uninitialized(int)
  ["b1"]=>
  uninitialized(?int)
}
