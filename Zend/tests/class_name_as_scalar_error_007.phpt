--TEST--
Cannot access self::class when no class scope is active
--FILE--
<?php

var_dump(self::class);

?>
--EXPECTF--
Fatal error: Cannot access self::class when no class scope is active in %s on line %d
