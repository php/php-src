--TEST--
Scalar type names cannot be used as class, trait or interface names (7)
--FILE--
<?php
namespace foo;

class int {}
?>
--EXPECTF--
Fatal error: Class "int" cannot be declared, int is a reserved class name in %s on line %d
