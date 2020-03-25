--TEST--
Default value not legal for any type in the union
--FILE--
<?php

class Test {
    public int|float $prop = "0";
}

?>
--EXPECTF--
Fatal error: Cannot use string as default value for property Test::$prop of type int|float in %s on line %d
