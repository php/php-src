--TEST--
Erasure: get_class returns the erased class name
--FILE--
<?php
class Box<T> {}
$b = new Box::<int>;
echo get_class($b), "\n";
?>
--EXPECT--
Box
