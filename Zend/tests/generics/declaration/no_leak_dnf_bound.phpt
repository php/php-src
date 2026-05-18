--TEST--
Generics: declaring a class with a DNF-typed bound does not leak memory
--FILE--
<?php
interface A {}
interface B {}
interface C {}

class Holder<T: (A&B)|C> {
    public function take(T $x): void {}
    public function get(): T {}
}

echo "ok\n";
?>
--EXPECT--
ok
