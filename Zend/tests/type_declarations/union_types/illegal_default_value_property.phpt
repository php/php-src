--TEST--
Default value not legal for any type in the union
--FILE--
<?php

class Test {
    public int|float $prop = "0";
}

?>
--EXPECTF--
Fatal error: Property string::$Test of type prop cannot have a default value of type int|float in %s on line %d
