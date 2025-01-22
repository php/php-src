--TEST--
#[\Deprecated]: Enum Cases.
--FILE--
<?php

enum E {
	#[\Deprecated]
	case Test;

	#[\Deprecated("use E::Test instead")]
	case Test2;
}

E::Test;
E::Test2;

?>
--EXPECTF--
Deprecated: Enum case E::Test is deprecated in %s on line %d

Deprecated: Enum case E::Test2 is deprecated, use E::Test instead in %s on line %d
