--TEST--
Decorated property without type
--FILE--
<?php

class Test {
    public decorated $prop;
}

?>
--EXPECTF--
Fatal error: Decorator property must have exactly one class type in %s on line %d
