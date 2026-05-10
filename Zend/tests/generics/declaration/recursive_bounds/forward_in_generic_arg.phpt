--TEST--
Recursive bounds: forward reference nested inside generic type argument
--FILE--
<?php
class Box<X> {}
function g<U: Box<T>, T>(U $x): void {}
echo "ok\n";
?>
--EXPECT--
ok
