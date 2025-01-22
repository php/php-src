--TEST--
Scalar type names cannot be used as class, trait or interface names (6) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "bool");
?>
--EXPECTF--
Fatal error: Cannot use "bool" as a class alias as it is reserved in %s on line %d
