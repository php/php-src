--TEST--
Abstract generic type in trait is invalid
--FILE--
<?php

trait C<T> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Fatal error: Cannot declare generic type on a trait in %s on line %d
