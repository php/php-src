--TEST--
declare(strict_namespace=1) must be the first statement in the script
--FILE--
<?php
$x = 1;
declare(strict_namespace=1);
?>
--EXPECTF--
Fatal error: strict_namespace declaration must be the very first statement in the script in %s on line %d
