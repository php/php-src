--TEST--
Recursive bounds: three-way cycle T -> U -> V -> T
--FILE--
<?php
class Box<X> {}
class Cycle<T: Box<U>, U: Box<V>, V: Box<T>> {}
echo "ok\n";
?>
--EXPECT--
ok
