--TEST--
Scalar type hint names cannot be used as class, trait or interface names (3) - class_alias
--FILE--
<?php

class foobar {}
class_alias("foobar", "float");
--EXPECTF--
Fatal error: Cannot use 'float' as class name as it is reserved in %s on line %d
