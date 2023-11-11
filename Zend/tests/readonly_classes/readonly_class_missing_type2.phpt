--TEST--
Promoted properties of a readonly class must have type
--FILE--
<?php

readonly class Foo
{
    public function __construct(
        private $bar
    ) {}
}

?>
--EXPECTF--
Fatal error: Readonly property Foo::$bar must have type in %s on line %d
