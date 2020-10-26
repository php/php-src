--TEST--
Scalar type names cannot be used as class, trait or interface names (6)
--FILE--
<?php

class bool {}
?>
--EXPECTF--
Fatal error: Class "bool" cannot be declared, bool is a reserved class name in %s on line %d
