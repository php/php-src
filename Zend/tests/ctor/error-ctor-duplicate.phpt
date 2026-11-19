--TEST--
Disallow duplicate short and full ctors
--FILE--
<?php

class DTO (
	public int $number,
) {
	public function __construct() {}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare DTO::__construct() in %s on line %d
