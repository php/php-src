--TEST--
Default value not legal for any type in the union
--FILE--
<?php

class Test {
    public int|float $prop = "0";
}

?>
--EXPECTF--
Fatal error: Property Test::$prop of type int|float cannot have a default value of type string in %s on line %d
