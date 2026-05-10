--TEST--
Errors: type parameter cannot be used at runtime in `catch`
--FILE--
<?php
function f<T>(): void {
    try { /* ... */ } catch (T $e) {}
}
?>
--EXPECTF--
Fatal error: Cannot use generic type parameter T as a class reference at runtime; bound-erased generic types have no runtime representation in %s on line %d
