--TEST--
Scalar type hint names cannot be used as class, trait or interface names (1) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "integer");
--EXPECTF--
Fatal error: "integer" cannot be used as a class name in %s on line %d
