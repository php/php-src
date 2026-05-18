--TEST--
Inheritance arity: no args is OK when all type parameters have defaults
--FILE--
<?php
class Box<T = mixed> {}
interface I<K = mixed, V = mixed> {}
trait Holder<T = mixed> { public T $v; }

class A extends Box {}
class B implements I {}
class C { use Holder; }

echo "OK\n";
?>
--EXPECT--
OK
