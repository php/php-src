--TEST--
#[\NoDiscard]: Throwing error handler.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

#[\NoDiscard]
function test(): int {
	return 0;
}

try {
	test();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

#[\NoDiscard]
function test2(): stdClass {
	return new stdClass();
}

try {
	test2();
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Caught: The return value of function test() should either be used or intentionally ignored by casting it as (void)
Caught: The return value of function test2() should either be used or intentionally ignored by casting it as (void)
