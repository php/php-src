--TEST--
#[\Deprecated]: Code is E_USER_DEPRECATED for class constants.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	var_dump($errno, E_USER_DEPRECATED, $errno === E_USER_DEPRECATED);
});

class Clazz {
	#[\Deprecated]
	public const TEST = 1;
}

Clazz::TEST;

?>
--EXPECT--
int(16384)
int(16384)
bool(true)
