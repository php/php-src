--TEST--
Recursive bounds: chain of forward references through three parameters
--FILE--
<?php
class Box<X> {}
class Foo {}
function f<T: Box<U>, U: Box<V>, V: Foo>(T $a, U $b, V $c): void {}
echo "ok\n";
?>
--EXPECT--
ok
