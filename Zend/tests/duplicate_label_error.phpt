--TEST--
Duplicate labels are not allowed
--FILE--
<?php

foo:
foo:
goto foo;

?>
--EXPECTF--
Fatal error: Label foo has already been defined in %s on line %d
