--TEST--
Abstract generic type in class is invalid (with class modifier)
--FILE--
<?php

abstract class C<T> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot declare generic type on a class in %s on line %d
