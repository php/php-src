--TEST--
Accessing self::FOO outside a class
--FILE--
<?php
var_dump(self::FOO);
?>
--EXPECTF--
Fatal error: Cannot use "self" when no class scope is active in %s on line %d
