--TEST--
Trying use name of an internal class as interface name
--FILE--
<?php

interface stdClass { }

?>
--EXPECTF--
Fatal error: interface stdClass has already been declared in %s on line %d
