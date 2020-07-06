--TEST--
Testing __set implementation with wrong declaration
--FILE--
<?php

abstract class b {
    abstract function __set($a, $b);
}

class a extends b {
    private function __set($a, $b) {
    }
}

?>
--EXPECTF--
Warning: Method a::__set() must have public visibility in %s on line %d

Fatal error: Method a::__set() must have public visibility to be compatible with overridden method b::__set() in %s on line %d
