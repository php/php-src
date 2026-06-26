--TEST--
Generics: declaring a class with an intersection-typed bound does not leak memory
--FILE--
<?php
interface A {}
interface B {}

class Holder<T: A&B> {
    public function take(T $x): void {}
}

echo "ok\n";
?>
--EXPECT--
ok
