--TEST--
Variance: out T cannot appear at an invariant slot of self<...> when the class param is invariant
--FILE--
<?php
class C<out T, U> {
    public function get(): self<T, T> {}
}
?>
--EXPECTF--
Fatal error: Type parameter T declared covariant (out T) cannot appear in invariant position in %s on line %d
