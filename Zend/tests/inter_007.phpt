--TEST--
Trying inherit abstract function twice
--FILE--
<?php

interface d {
    static function B();
}

interface c {
    function b();
}

class_alias('c', 'w');

interface a extends d, w { }

?>
--EXPECTF--
Fatal error: Method d::B() must not be static to be compatible with overridden method c::B() in %s on line %d
