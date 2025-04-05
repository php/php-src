--TEST--
Associated types basic
--FILE--
<?php

interface I {
    type T;
    public function foo(T $param): T;
}

class C implements I {
    public function foo(string $param): string {}
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "type", expecting "function" in %s on line %d
