--TEST--
declare(strict_namespace=1) must not use block mode
--FILE--
<?php
declare(strict_namespace=1) {
}
?>
--EXPECTF--
Fatal error: strict_namespace declaration must not use block mode in %s on line %d
