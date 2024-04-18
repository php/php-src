--TEST--
Trying use name of an internal class as interface name
--FILE--
<?php

interface stdClass { }

?>
--EXPECTF--
Fatal error: Cannot declare interface stdClass, because the name is already in use (previously declared in %s:%d) in %s on line %d
