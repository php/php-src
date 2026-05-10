--TEST--
Recursive bounds: four-parameter cycle A -> B -> C -> D -> A
--FILE--
<?php
class Box<X> {}
class Quad<A: Box<B>, B: Box<C>, C: Box<D>, D: Box<A>> {}
echo "ok\n";
?>
--EXPECT--
ok
