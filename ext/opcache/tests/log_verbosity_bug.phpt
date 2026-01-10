--TEST--
Test ACCEL_LOG_FATAL will cause the process to die even if not logged
--DESCRIPTION--
This test forces the opcache to error by setting file_cache_only without file_cache.
The resulting ACCEL_LOG_FATAL should cause php to die.
The process should die regardless of the log_verbosity_level.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache=
opcache.file_cache_only=1
opcache.log_verbosity_level=-1
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xleak Startup failure leak');
?>
--FILE--
<?php
var_dump("Script should fail");
?>
--EXPECT--
