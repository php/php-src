--TEST--
Trying use name of an internal class as interface name
--FILE--
<?php

interface StdClass { }

?>
--EXPECTF--
Fatal error: Cannot declare interface StdClass, because the name is already in use in %s on line %d
