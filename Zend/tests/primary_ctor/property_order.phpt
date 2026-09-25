--TEST--
Primary constructors: promoted properties are declared before body members
--FILE--
<?php
class Mixed1(public int $a, public int $b) {
    public int $c = 3;
}

$m = new Mixed1(1, 2);
var_dump($m);

class Empty1() {}
var_dump((new ReflectionClass(Empty1::class))->getConstructor()->getNumberOfParameters());
?>
--EXPECT--
object(Mixed1)#1 (3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
int(0)
