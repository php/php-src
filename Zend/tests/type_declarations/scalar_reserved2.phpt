--TEST--
Scalar type names cannot be used as class, trait or interface names (2)
--FILE--
<?php

class int {}
?>
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d
