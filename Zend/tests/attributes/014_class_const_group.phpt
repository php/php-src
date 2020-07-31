--TEST--
Attributes cannot be applied to groups of class constants.
--FILE--
<?php

class C1
{
	@[A1]
	public const A = 1, B = 2;
}

?>
--EXPECTF--
Fatal error: Cannot apply attributes to a group of constants in %s
