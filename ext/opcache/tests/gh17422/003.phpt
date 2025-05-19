--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - Fatal Error
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
	str_repeat('x', 1024 * 1024 * 1024);
});

require __DIR__ . "/warning.inc";

warning();

?>
--EXPECTF--
Fatal error: Allowed memory size of 2097152 bytes exhausted %s on line 6
array(2) {
  [0]=>
  string(7) "003.php"
  [1]=>
  string(12) "shutdown.inc"
}
