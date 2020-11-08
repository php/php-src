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
Fatal error: Method a::__toString() cannot take arguments in %s on line %d
