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
?>
--EXPECTF--
Fatal error: Type of Baz::$bar must be omitted to match the parent definition in class Foo in %s on line 6
