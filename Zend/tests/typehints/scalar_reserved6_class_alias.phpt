--TEST--
Scalar type hint names cannot be used as class, trait or interface names (6) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "bool");
--EXPECTF--
Fatal error: "bool" cannot be used as a class name in %s on line %d
