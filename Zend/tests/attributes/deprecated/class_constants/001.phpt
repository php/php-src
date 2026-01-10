--TEST--
#[\Deprecated]: Class Constants.
--FILE--
<?php

class Clazz {
	#[\Deprecated]
	public const TEST = 1;

	#[\Deprecated()]
	public const TEST2 = 2;

	#[\Deprecated("use Clazz::TEST instead")]
	public const TEST3 = 3;

	#[\Deprecated]
	public const TEST4 = 4;

	#[\Deprecated]
	public const TEST5 = 5;
}

var_dump(Clazz::TEST);
var_dump(Clazz::TEST2);
var_dump(Clazz::TEST3);

var_dump(constant('Clazz::TEST4'));
var_dump(defined('Clazz::TEST5'));

?>
--EXPECTF--
Deprecated: Constant Clazz::TEST is deprecated in %s on line %d
int(1)

Deprecated: Constant Clazz::TEST2 is deprecated in %s on line %d
int(2)

Deprecated: Constant Clazz::TEST3 is deprecated, use Clazz::TEST instead in %s on line %d
int(3)

Deprecated: Constant Clazz::TEST4 is deprecated in %s on line %d
int(4)
bool(true)
