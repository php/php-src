--TEST--
Testing each() with an undefined variable
--FILE--
<?php

each($foo);

?>
--EXPECTF--
Warning: Variable passed to each() is not an array or object in %s on line %d
