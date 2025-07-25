--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations)
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php

require __DIR__ . "/shutdown.inc";

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	echo "set_error_handler: {$errstr}", PHP_EOL;
});

require __DIR__ . "/warning.inc";

warning();

?>
--EXPECT--
set_error_handler: "continue" targeting switch is equivalent to "break"
OK: warning
array(3) {
  [0]=>
  string(7) "001.php"
  [1]=>
  string(12) "shutdown.inc"
  [2]=>
  string(11) "warning.inc"
}
