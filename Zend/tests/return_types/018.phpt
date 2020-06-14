--TEST--
Destructors must return void
--FILE--
<?php

class Foo {
    function __destruct() : Foo {}
}
--EXPECTF--
Fatal error: Destructor %s::%s() must return void in %s on line %d
