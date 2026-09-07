--TEST--
#[\Deprecated]: Throwing error handler when using the return value.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

#[Deprecated]
function test() {}

try {
	$x = test();
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ErrorException: Function test() is deprecated
