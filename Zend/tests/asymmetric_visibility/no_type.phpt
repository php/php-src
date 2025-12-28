--TEST--
Asymmetric visibility with no type
--FILE--
<?php

class Foo {
    public private(set) $bar;
}

?>
--EXPECTF--
Fatal error: Property with asymmetric visibility Foo::$bar must have type in %s on line %d
