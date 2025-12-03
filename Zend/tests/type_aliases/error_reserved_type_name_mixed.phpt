--TEST--
Type alias cannot use reserved type name (mixed)
--FILE--
<?php
use type int|string as mixed;
?>
--EXPECTF--
Fatal error: Cannot use 'mixed' as type alias name as it is reserved in %s on line %d
