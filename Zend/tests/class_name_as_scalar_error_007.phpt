--TEST--
Cannot access self::class when no class scope is active
--FILE--
<?php

var_dump(self::class);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use "self" when no class scope is active in %s:3
Stack trace:
#0 {main}
  thrown in %s on line 3
