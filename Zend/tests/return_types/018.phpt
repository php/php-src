--TEST--
Destructors cannot declare a return type
--FILE--
<?php

class Foo {
    function __destruct() : Foo {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__destruct() cannot declare a return type in %s on line %d
