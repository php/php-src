--TEST--
Type alias cannot use reserved type name (int)
--FILE--
<?php
use type int|float as int;
?>
--EXPECTF--
Fatal error: Cannot use 'int' as type alias name as it is reserved in %s on line %d
