--TEST--
Duplicate labels are not allowed
--FILE--
<?php

foo:
foo:
goto foo;

?>
--EXPECTF--
Fatal error: Label foo cannot be redefined in %s on line %d
