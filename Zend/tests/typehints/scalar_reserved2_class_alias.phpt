--TEST--
Scalar type hint names cannot be used as class, trait or interface names (2) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "int");
--EXPECTF--
Fatal error: "int" cannot be used as a class name in %s on line %d
