--TEST--
Variance: out T as the type-arg to a contravariant parent's parameter is rejected
--FILE--
<?php
class Cons<in U> {}

class A<out T> extends Cons<T> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
