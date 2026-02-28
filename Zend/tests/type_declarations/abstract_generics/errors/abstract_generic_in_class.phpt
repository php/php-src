--TEST--
Abstract generic type in class is invalid
--FILE--
<?php

class C<T> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
--EXPECTF--
Fatal error: Cannot declare generic type on a class in %s on line %d
