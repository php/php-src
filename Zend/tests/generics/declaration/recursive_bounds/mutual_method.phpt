--TEST--
Recursive bounds: method with mutually recursive type parameters
--FILE--
<?php
class Box<T> {}
class C {
    public function f<T: Box<U>, U: Box<T>>(T $x, U $y): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
