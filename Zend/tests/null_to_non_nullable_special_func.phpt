--TEST--
Test null arg behavior for special functions
--FILE--
<?php

$null = null;
var_dump(strlen($null));

?>
--EXPECTF--
Deprecated: strlen(): Passing null to argument of type string is deprecated in %s on line %d
int(0)
