--TEST--
Scalar type names cannot be used as class, trait or interface names (4)
--FILE--
<?php

class string {}
?>
--EXPECTF--
Fatal error: Cannot use 'string' as class name as it is reserved in %s on line %d
