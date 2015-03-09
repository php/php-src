--TEST--
Accessing self::FOO outside a class
--FILE--
<?php
var_dump(self::FOO);
?>
--EXPECTF--
Fatal error: Cannot access self:: when no class scope is active in %s on line %d
