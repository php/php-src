--TEST--
Duplicate labels are not allowed
--FILE--
<?php

foo:
foo:
goto foo;

?>
--EXPECTF--
Fatal error: Label 'foo' already defined in %s on line %d
