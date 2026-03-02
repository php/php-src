--TEST--
#[\NoDiscard]: Code is E_USER_WARNING.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	var_dump($errno, E_USER_WARNING, $errno === E_USER_WARNING);
});

#[\NoDiscard]
function test(): int {
	return 0;
}

test();

?>
--EXPECT--
int(512)
int(512)
bool(true)
