--TEST--
Test typed properties initial values
--FILE--
<?php
class Foo {
    public int $int = 1;
    public float $flt = 2.2;
    public float $flt2 = 2;
    public array $arr = [];
    public bool $bool = false;
    public iterable $iter = [];
}
var_dump(new Foo);
?>
--EXPECT--
object(Foo)#1 (6) {
  ["int"]=>
  int(1)
  ["flt"]=>
  float(2.2)
  ["flt2"]=>
  float(2)
  ["arr"]=>
  array(0) {
  }
  ["bool"]=>
  bool(false)
  ["iter"]=>
  array(0) {
  }
}
