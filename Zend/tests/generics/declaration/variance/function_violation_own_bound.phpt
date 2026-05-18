--TEST--
Variance: +T on a function appearing in its own bound (invariant position) is rejected
--FILE--
<?php
interface Box<U> {}

function f<+T : Box<T>>(): void {}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
