--TEST--
GH-17422 (OPcache bypasses the user-defined error handler for deprecations) - File cache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache="{TMP}"
opcache.file_cache_only=1
opcache.record_warnings=1
--EXTENSIONS--
opcache
--FILE--
<?php

set_error_handler(static function (int $errno, string $errstr, string $errfile, int $errline) {
	echo "set_error_handler: {$errstr}", PHP_EOL;
});

require __DIR__ . "/warning.inc";

warning();

?>
--EXPECT--
set_error_handler: "continue" targeting switch is equivalent to "break"
OK: warning
