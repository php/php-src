--TEST--
Scalar type hint names cannot be used as class, trait or interface names (3) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "float");
--EXPECTF--
Fatal error: "float" cannot be used as a class name in %s on line %d
