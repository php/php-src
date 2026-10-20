--TEST--
Testing __callstatic declaration with wrong modifier
--FILE--
<?php

class a {
    static protected function __callstatic($a, $b) {
    }
}

?>
--EXPECTF--
Warning: The magic method a::__callstatic() must have public visibility in %s on line %d
