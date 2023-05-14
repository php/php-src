--TEST--
Closure self-reference can't use this as a variable name
--FILE--
<?php

function() as $this {};

?>
--EXPECTF--
Fatal error: Cannot use $this as self reference in %s on line %d
