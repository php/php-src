--TEST--
Testing __callstatic declaration in interface with missing the 'static' modifier
--FILE--
<?php

interface a {
    function __callstatic($a, $b);
}

?>
--EXPECTF--
Warning: The magic method a::__callStatic() must be static in %s on line %d
