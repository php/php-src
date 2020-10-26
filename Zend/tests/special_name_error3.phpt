--TEST--
Cannot use special class name as trait name
--FILE--
<?php

trait self {}

?>
--EXPECTF--
Fatal error: Class "self" cannot be declared, self is a reserved class name in %s on line %d
