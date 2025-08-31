--TEST--
Constructors cannot declare a return type
--FILE--
<?php

class Foo {
    function __construct() : Foo {}
}
?>
--EXPECTF--
Fatal error: Method Foo::__construct() cannot declare a return type in %s on line %d
