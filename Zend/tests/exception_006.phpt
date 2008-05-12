--TEST--
Trying to throw a non-object
--FILE--
<?php

throw 1;

?>
--EXPECTF--
Fatal error: Can only throw objects in %s on line %d
