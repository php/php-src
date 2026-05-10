--TEST--
Recursive bounds: outer-scope class param appears inside mutually recursive method bounds
--FILE--
<?php
class Box<X> {}
class Carrier<O: Box<O>> {
    public function pair<T: Box<U>, U: Box<T>>(T $a, U $b, O $c): void {}
}
echo "ok\n";
?>
--EXPECT--
ok
