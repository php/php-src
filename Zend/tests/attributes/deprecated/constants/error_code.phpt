--TEST--
#[\Deprecated]: Code is E_USER_DEPRECATED for constants.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	var_dump($errno, E_USER_DEPRECATED, $errno === E_USER_DEPRECATED);
});

#[\Deprecated]
const EXAMPLE = 5;

EXAMPLE;

?>
--EXPECT--
int(16384)
int(16384)
bool(true)
