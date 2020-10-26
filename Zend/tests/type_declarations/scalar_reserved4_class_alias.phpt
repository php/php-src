--TEST--
Scalar type names cannot be used as class, trait or interface names (4) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "string");
?>
--EXPECTF--
Fatal error: Class "string" cannot be declared, string is a reserved class name in %s on line %d
