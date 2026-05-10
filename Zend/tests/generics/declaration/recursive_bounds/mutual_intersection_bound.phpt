--TEST--
Recursive bounds: mutual references inside intersection bounds
--FILE--
<?php
interface Box<X> {}
interface Tagged {}
class Pair<T: Box<U>&Tagged, U: Box<T>&Tagged> {}
echo "ok\n";
?>
--EXPECT--
ok
