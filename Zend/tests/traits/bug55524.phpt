--TEST--
Bug #55524 Traits should not be able to extend a class
--FILE--
<?php

class Base {}

trait Foo extends Base {
    function bar() {}
}

echo 'DONE';
?>
--EXPECTF--
Parse error: syntax error, unexpected 'extends' (T_EXTENDS), expecting '{' in %s on line %d
