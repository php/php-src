--TEST--
Scalar type hint names cannot be used as class, trait or interface names (2) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "int");
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d
