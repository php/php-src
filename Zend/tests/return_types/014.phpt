--TEST--
Constructors must return void
--FILE--
<?php

class Foo {
    function __construct() : Foo {}
}
--EXPECTF--
Fatal error: %s::%s(): Return type must be void when declared in %s on line %d
