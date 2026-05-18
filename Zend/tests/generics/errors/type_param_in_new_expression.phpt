--TEST--
Errors: type parameter cannot be used at runtime as a class in `new T()`
--FILE--
<?php
function f<T>(): void {
    $x = new T();
}
?>
--EXPECTF--
Fatal error: Cannot use generic type parameter T as a class reference at runtime; bound-erased generic types have no runtime representation in %s on line %d
