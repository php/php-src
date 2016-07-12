--TEST--
Test strict declaration block declaration 008
--FILE--
<?php
declare(strict_types=1) {
	var_dump(strlen("abc"));
}

?>
--EXPECTF--
Fatal error: strict_types declaration must not use block mode in %s on line %d