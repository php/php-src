--TEST--
Variance: function-origin out T composed through Consumer<in U> in return is rejected
--FILE--
<?php
interface Consumer<in U> { public function consume(U $x): void; }

function f<out T>(): Consumer<T> {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in contravariant position in %s on line %d
