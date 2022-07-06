--TEST--
Trying to inherit a class in an interface
--FILE--
<?php

interface a extends Exception { }

?>
--EXPECTF--
Fatal error: a cannot implement Exception - it is not an interface in %s on line %d
