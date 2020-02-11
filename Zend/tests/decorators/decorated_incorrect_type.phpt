--TEST--
Decorated property illegal type
--FILE--
<?php

class Test {
    public decorated ?Foo $prop;
}

?>
--EXPECTF--
Fatal error: Decorator property must have exactly one class type in %s on line %d
