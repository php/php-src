--TEST--
Testing __call() declaration in interface with wrong modifier
--FILE--
<?php

interface a {
    static function __call($a, $b);
}

?>
--EXPECTF--
Fatal error: Method a::__call() cannot be static in %s on line %d
