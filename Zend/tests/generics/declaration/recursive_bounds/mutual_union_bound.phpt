--TEST--
Recursive bounds: mutual references inside union bounds
--FILE--
<?php
class Box<X> {}
class Pair<T: Box<U>|null, U: Box<T>|null> {}
echo "ok\n";
?>
--EXPECT--
ok
