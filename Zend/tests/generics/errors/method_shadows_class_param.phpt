--TEST--
Errors: method type parameter shadows class type parameter
--FILE--
<?php
class Box<T> {
    public function f<T>(): void {}
}
?>
--EXPECTF--
Fatal error: Type parameter T shadows enclosing type parameter in %s on line %d
