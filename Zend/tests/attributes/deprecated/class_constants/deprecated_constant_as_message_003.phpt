--TEST--
#[\Deprecated]: Using the value of a deprecated class constant in a constant expression.
--FILE--
<?php

class Clazz {
	#[\Deprecated("prefix")]
	public const PREFIX = "prefix";

	#[\Deprecated("suffix")]
	public const SUFFIX = "suffix";

	public const CONSTANT = self::PREFIX . self::SUFFIX;
}

var_dump(Clazz::CONSTANT);

?>
--EXPECTF--
Deprecated: Constant Clazz::PREFIX is deprecated, prefix in %s on line %d

Deprecated: Constant Clazz::SUFFIX is deprecated, suffix in %s on line %d
string(12) "prefixsuffix"
