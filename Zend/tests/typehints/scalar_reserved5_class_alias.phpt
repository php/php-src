--TEST--
Scalar type hint names cannot be used as class, trait or interface names (5) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "boolean");
--EXPECTF--
Fatal error: "boolean" cannot be used as a class name in %s on line %d
