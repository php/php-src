--TEST--
Interface that is extended from Enum only interface shouldn't be implementable by non-enum class
--FILE--
<?php
interface I extends UnitEnum {}

class C implements I {
	public static function cases(): array {
		return [];
	}
}

?>
--EXPECTF--
Fatal error: Non-enum class C cannot implement interface I in %s on line %d
