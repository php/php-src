--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - require
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
	require_once __DIR__ . "/dummy.inc";
});

require __DIR__ . "/warning.inc";

dummy();

?>
--EXPECT--
OK: dummy
array(4) {
  [0]=>
  string(7) "005.php"
  [1]=>
  string(9) "dummy.inc"
  [2]=>
  string(12) "shutdown.inc"
  [3]=>
  string(11) "warning.inc"
}
