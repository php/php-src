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
Parse error: syntax error, unexpected token "extends", expecting "{" in %s on line %d
