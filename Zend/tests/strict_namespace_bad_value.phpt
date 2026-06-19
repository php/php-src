--TEST--
declare(strict_namespace=1) value must be 0 or 1
--FILE--
<?php
declare(strict_namespace=2);
?>
--EXPECTF--
Fatal error: strict_namespace declaration must have 0 or 1 as its value in %s on line %d
