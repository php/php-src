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
Fatal error: Access level to Bar::__construct() must be public (as in class Foo) in %s
