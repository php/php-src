--TEST--
Yield cannot be used outside of functions
--FILE--
<?php

yield "Test";

?>
--EXPECTF--
Fatal error: The "yield" expression can only be used inside a function in %s on line %d
