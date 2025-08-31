--TEST--
Normal properties of a readonly class must have type
--FILE--
<?php

readonly class Foo
{
    public $bar;
}

?>
--EXPECTF--
Fatal error: Readonly property Foo::$bar must have type in %s on line %d
