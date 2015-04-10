--TEST--
Scalar type hint names cannot be used as class, trait or interface names (6) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "bool");
--EXPECTF--
Fatal error: Cannot use 'bool' as class name as it is reserved in %s on line %d
