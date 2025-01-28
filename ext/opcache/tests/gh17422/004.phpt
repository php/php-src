--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - eval
--INI--
opcache.enable=1
opcache.enable_cli=1
memory_limit=2M
--EXTENSIONS--
opcache
--FILE--
<?php

require __DIR__ . "/shutdown.inc";

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
Fatal error: Cannot redeclare function warning() (previously declared in %s(8) : eval()'d code:1) in %swarning.inc on line 2
array(2) {
  [0]=>
  string(7) "004.php"
  [1]=>
  string(12) "shutdown.inc"
}
