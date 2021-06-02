--TEST--
Readonly property without type
--FILE--
<?php

class Test {
    protected readonly $prop;
}

?>
--EXPECTF--
Fatal error: Readonly property Test::$prop must have type in %s on line %d
