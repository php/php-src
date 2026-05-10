--TEST--
Erasure: var_dump shows erased class name
--FILE--
<?php
class Box<T> {
    public int $x = 1;
}
var_dump(new Box::<int>);
?>
--EXPECT--
object(Box)#1 (1) {
  ["x"]=>
  int(1)
}
