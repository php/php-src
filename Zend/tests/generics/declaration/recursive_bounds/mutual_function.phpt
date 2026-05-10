--TEST--
Recursive bounds: function with mutually recursive type parameters
--FILE--
<?php
class Box<T> {}
function f<T: Box<U>, U: Box<T>>(T $x, U $y): void {}
echo "ok\n";
?>
--EXPECT--
ok
