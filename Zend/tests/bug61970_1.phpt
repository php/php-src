--TEST--
Bug #61970 (Restraining __construct() access level in subclass gives a fatal error - stays when implementing abstract)
--FILE--
<?php

abstract class Foo {
    abstract public function __construct();
}

class Bar extends Foo {
    protected function __construct(){}
}
--EXPECTF--
Fatal error: Method Bar::__construct() must have public visibility to be compatible with overridden method Foo::__construct() in %s on line %d
