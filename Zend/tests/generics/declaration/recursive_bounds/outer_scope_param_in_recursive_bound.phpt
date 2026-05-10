--TEST--
Recursive bounds: an enclosing class's type parameter can appear inside a recursive method bound
--FILE--
<?php
class Box<X> {}
class C<O> {
    public function f<T: Box<U>, U: Box<T>>(T $a, U $b, O $c): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
