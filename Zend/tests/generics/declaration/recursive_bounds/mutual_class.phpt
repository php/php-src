--TEST--
Recursive bounds: class with mutually recursive type parameters
--FILE--
<?php
class Box<T> {}
class Pair<T: Box<U>, U: Box<T>> {}
echo "ok\n";
?>
--EXPECT--
ok
