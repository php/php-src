--TEST--
Testing __callStatic declaration in interface with missing the 'static' modifier
--FILE--
<?php

interface a {
    function __callStatic($a, $b);
}

?>
--EXPECTF--
Fatal error: Method a::__callStatic() must be static in %s on line %d
