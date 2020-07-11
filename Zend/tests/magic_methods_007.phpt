--TEST--
Testing __set() declaration in abstract class with wrong modifier
--FILE--
<?php

abstract class b {
    abstract protected function __set($a);
}

?>
--EXPECTF--
Warning: The magic method b::__set() must have public visibility in %s on line %d

Fatal error: Method b::__set() must take exactly 2 arguments in %s on line %d
