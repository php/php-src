--TEST--
Enum disallows constructor
--FILE--
<?php

enum Foo {
    public function __construct() {}
}

?>
--EXPECTF--
Fatal error: Enum may not include magic method __construct in %s on line %d
