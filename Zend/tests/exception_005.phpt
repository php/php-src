--TEST--
Trying to throw exception of an interface
--FILE--
<?php

interface a { }

throw new a();

?>
--EXPECTF--
Fatal error: Cannot instantiate interface a in %s on line %d
