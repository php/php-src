--TEST--
Errors: recursive bound (T : Comparable<T>) is allowed (positive test)
--FILE--
<?php
interface Comparable<U> {}
function f<T : Comparable<T>>(T $x): T { return $x; }
echo "ok\n";
?>
--EXPECT--
ok
