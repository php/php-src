--TEST--
Type alias cannot use reserved type name (void)
--FILE--
<?php
use type int as void;
?>
--EXPECTF--
Fatal error: Cannot use 'void' as type alias name as it is reserved in %s on line %d
