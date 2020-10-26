--TEST--
Scalar type names cannot be used as class, trait or interface names (6) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "bool");
?>
--EXPECTF--
Fatal error: Class "bool" cannot be declared, bool is a reserved class name in %s on line %d
