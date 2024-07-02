--TEST--
#[\Deprecated]: Throwing error handler does not leak parameters.
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, ?string $errfile = null, ?int $errline = null) {
	throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

#[Deprecated]
function test($dummy) {}

try {
	$x = test(new stdClass());
} catch (ErrorException $e) {
	echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Caught: Function test() is deprecated
