--TEST--
Yield cannot be used outside of functions
--FILE--
<?php

yield "Test";

?>
--EXPECTF--
Fatal error: The "yield" statement can only be used inside a generator function in %s on line %d
