--TEST--
Trying use name of an internal class as interface name
--FILE--
<?php

interface stdClass { }

?>
--EXPECTF--
Fatal error: Cannot redeclare class stdClass in %s on line %d
