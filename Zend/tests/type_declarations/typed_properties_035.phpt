--TEST--
Test typed properties inheritance must not change type
--FILE--
<?php
class Foo{
    public $bar = 42;
}

class Baz extends Foo{
    public int $bar = 33;
}
--EXPECTF--
Fatal error: Property Baz::$bar must not have a type to be compatible with overridden property Foo::$bar in %s on line %d
