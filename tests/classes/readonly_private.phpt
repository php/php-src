--TEST--
Ensure readonly private properties are not allowed
--FILE--
<?php

class Foo {
    readonly private $foo;
}
--EXPECTF--
Fatal error: Properties cannot be both readonly and private in %s on line %d
