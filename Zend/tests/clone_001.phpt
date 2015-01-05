--TEST--
Using clone statement on non-object
--FILE--
<?php

$a = clone array();

?>
--EXPECTF--
Fatal error: __clone method called on non-object in %s on line %d
