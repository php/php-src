--TEST--
__clone can only declare void return
--FILE--
<?php

class Foo {
    function __clone() : Foo {}
}
?>
--EXPECTF--
Fatal error: Foo::__clone(): Return type must be void when declared in %s on line %d
