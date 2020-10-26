--TEST--
Scalar type names cannot be used as class, trait or interface names (2) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "int");
?>
--EXPECTF--
Fatal error: Class "int" cannot be declared, int is a reserved class name in %s on line %d
