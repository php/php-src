--TEST--
Abstract generic type in trait is invalid
--FILE--
<?php

trait C<T> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "<", expecting "{" in %s on line %d
