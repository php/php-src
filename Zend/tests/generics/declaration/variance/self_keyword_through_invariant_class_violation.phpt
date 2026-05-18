--TEST--
Variance: +T cannot appear at an invariant slot of self<...> when the class param is invariant
--FILE--
<?php
class C<+T, U> {
    public function get(): self<T, T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (+T) cannot appear in invariant position in %s on line %d
