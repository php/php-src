--TEST--
Promoted readonly property must have type
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly $bar
    ) {}
}

?>
--EXPECTF--
Fatal error: Readonly property Foo::$bar must have type in %s on line %d
