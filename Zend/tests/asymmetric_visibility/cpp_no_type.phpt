--TEST--
Asymmetric visibility in CPP with no type
--FILE--
<?php

class Foo {
    public function __construct(
        public private(set) $bar,
    ) {}
}

?>
--EXPECTF--
Fatal error: Property with asymmetric visibility Foo::$bar must have type in %s on line %d
