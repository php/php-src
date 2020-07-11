--TEST--
Testing __toString() declaration with wrong modifier
--FILE--
<?php

class a {
    static protected function __toString($a, $b) {
    }
}

?>
--EXPECTF--
Warning: The magic method a::__toString() must have public visibility in %s on line %d

Warning: The magic method a::__toString() cannot be static in %s on line %d

Fatal error: Method a::__toString() cannot take arguments in %s on line %d
