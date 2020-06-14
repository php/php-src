--TEST--
Constructors must return void
--FILE--
<?php

class Foo {
    function __construct() : Foo {}
}
--EXPECTF--
Fatal error: Constructor %s::%s() must return void in %s on line %d
