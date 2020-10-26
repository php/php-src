--TEST--
ZE2 An interface method must be abstract
--FILE--
<?php

interface if_a {
    function err() {}
}

?>
--EXPECTF--
Fatal error: Interface method if_a::err() cannot have a body in %s on line %d
