--TEST--
Variance: +T as the type-arg to a contravariant parent's parameter is rejected
--FILE--
<?php
class Cons<-U> {}

class A<+T> extends Cons<T> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in contravariant position in %s on line %d
