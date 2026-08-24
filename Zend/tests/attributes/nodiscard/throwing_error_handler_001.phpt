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
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

#[\NoDiscard]
function test2(): stdClass {
	return new stdClass();
}

try {
	test2();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ErrorException: The return value of function test() should either be used or intentionally ignored by casting it as (void)
ErrorException: The return value of function test2() should either be used or intentionally ignored by casting it as (void)
