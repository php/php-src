--TEST--
Scalar type names cannot be used as class, trait or interface names (3)
--FILE--
<?php

class float {}
?>
--EXPECTF--
Fatal error: Cannot use 'float' as class name as it is reserved in %s on line %d
