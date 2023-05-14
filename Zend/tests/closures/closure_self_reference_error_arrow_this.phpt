--TEST--
Closure self-reference can't use this as a variable name (arrow function)
--FILE--
<?php

fn() as $this => 1;

?>
--EXPECTF--
Fatal error: Cannot use $this as self reference in %s on line %d
