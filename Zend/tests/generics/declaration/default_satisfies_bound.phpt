--TEST--
Declaration: type-parameter default that satisfies its bound is accepted
--FILE--
<?php
class A {}
class B extends A {}

class Box<T : A = B> {}
function f<T : A = B>(): void {}
trait Tr<T : A = B> {}
interface I<T : A = B> {}

new Box;
f();
echo "OK\n";
?>
--EXPECT--
OK
