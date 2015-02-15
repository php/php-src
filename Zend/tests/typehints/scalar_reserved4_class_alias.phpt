--TEST--
Scalar type hint names cannot be used as class, trait or interface names (4) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "string");
--EXPECTF--
Fatal error: "string" cannot be used as a class name in %s on line %d
