--TEST--
Bug #61970 (Restraining __construct() access level in subclass gives a fatal error - stays when inheriting implemented abstract)
--FILE--
<?php

abstract class Foo {
    abstract public function __construct();
}

class Bar extends Foo {
    public function __construct(){}
}

class Baz extends Bar {
    protected function __construct(){}
}
--EXPECTF--
Fatal error: Method Baz::__construct() must have public visibility to be compatible with overridden method Foo::__construct() in %s on line %d
