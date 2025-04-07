--TEST--
#[\Deprecated]: Using the value of a deprecated class constant as the deprecation message.
--FILE--
<?php

class Clazz {
	#[\Deprecated(self::TEST)]
	public const TEST = "from itself";

	#[\Deprecated]
	public const TEST2 = "from another";

	#[\Deprecated(self::TEST2)]
	public const TEST3 = 1;
}

Clazz::TEST;
Clazz::TEST3;

?>
--EXPECTF--
Deprecated: Constant Clazz::TEST is deprecated, from itself in %s on line %d

Deprecated: Constant Clazz::TEST2 is deprecated in %s on line %d

Deprecated: Constant Clazz::TEST3 is deprecated, from another in %s on line %d
