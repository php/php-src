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
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Caught: Function test() is deprecated
