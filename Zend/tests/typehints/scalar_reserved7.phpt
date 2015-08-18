--TEST--
Scalar type hint names cannot be used as class, trait or interface names (7)
--FILE--
<?php
namespace foo;

class int {}
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d
