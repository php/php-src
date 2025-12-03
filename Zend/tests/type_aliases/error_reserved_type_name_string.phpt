--TEST--
Type alias cannot use reserved type name (string)
--FILE--
<?php
use type int as string;
?>
--EXPECTF--
Fatal error: Cannot use 'string' as type alias name as it is reserved in %s on line %d
