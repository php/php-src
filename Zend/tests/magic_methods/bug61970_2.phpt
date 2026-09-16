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
?>
--EXPECTF--
Fatal error: Access level to Baz::__construct() must be public (as in class Foo) in %s on line 12
