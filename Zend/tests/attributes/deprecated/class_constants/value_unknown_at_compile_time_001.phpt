--TEST--
#[\Deprecated]: Class constant with value unknown at compile time.
--FILE--
<?php

define('SUFFIX', random_int(1, 2) == 1 ? 'a' : 'b');

class Clazz {
	#[\Deprecated]
	public const CONSTANT = self::class . '-' . SUFFIX;
}

$value = Clazz::CONSTANT;
var_dump($value);
var_dump($value === 'Clazz-' . SUFFIX);

?>
--EXPECTF--
Deprecated: Constant Clazz::CONSTANT is deprecated in %s on line %d
string(7) "Clazz-%c"
bool(true)
