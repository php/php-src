--TEST--
Attributes cannot be applied to groups of properties.
--FILE--
<?php

class C1
{
	@[A1]
	public $x, $y;
}

?>
--EXPECTF--
Fatal error: Cannot apply attributes to a group of properties in %s
