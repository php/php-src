--TEST--
DNF type in trait
--FILE--
<?php

trait A {
    public A|(X&Y)|Traversable $v;
}

class C {
    use A;
}

$o = new C();
var_dump($o);

?>
===DONE===
--EXPECT--
object(C)#1 (0) {
  ["v"]=>
  uninitialized(A|(X&Y)|Traversable)
}
===DONE===
