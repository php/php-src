--TEST--
Scalar type names cannot be used as class, trait or interface names (6)
--FILE--
<?php

class bool {}
?>
--EXPECTF--
Fatal error: Cannot use 'bool' as class name as it is reserved in %s on line %d
