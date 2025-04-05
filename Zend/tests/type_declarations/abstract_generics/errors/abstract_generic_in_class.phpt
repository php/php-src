--TEST--
Abstract generic type in class is invalid
--FILE--
<?php

class C<T> {
    public function foo(T $param): T;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "<", expecting "{" in %s on line %d
