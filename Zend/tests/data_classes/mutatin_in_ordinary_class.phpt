--TEST--
Mutating keyword in ordinary class
--FILE--
<?php

class Point {
    public mutating function zero() {}
}

?>
--EXPECTF--
Fatal error: Mutating modifier may only be added to data class methods in %s on line %d
