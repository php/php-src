--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - eval
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	eval(
		<<<'PHP'
			function warning() {
				echo "NOK", PHP_EOL;
			}
			PHP
	);
});

require __DIR__ . "/warning.inc";

warning();

?>
--EXPECTF--
Fatal error: Cannot redeclare function warning() %s
