--TEST--
Scalar type names cannot be used as class, trait or interface names (3)
--FILE--
<?php

class float {}
?>
--EXPECTF--
Fatal error: Class "float" cannot be declared, float is a reserved class name in %s on line %d
