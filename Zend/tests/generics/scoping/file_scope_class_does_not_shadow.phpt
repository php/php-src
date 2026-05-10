--TEST--
Scoping: a file-scope class does NOT shadow a generic type parameter
--FILE--
<?php
class T {}
function f<T>(): void {
    new T();
}
?>
--EXPECTF--
Fatal error: Cannot use generic type parameter T as a class reference at runtime; bound-erased generic types have no runtime representation in %s on line %d
