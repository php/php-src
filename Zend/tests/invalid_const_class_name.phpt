--TEST--
Invalid constant class name in nested class constant access
--FILE--
<?php
[]::X::X;
?>
--EXPECTF--
Fatal error: Illegal class name in %s on line %d
