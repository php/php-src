--TEST--
Test ACCEL_LOG_FATAL will cause the process to die even if not logged
--DESCRIPTION--
This test forces the opcache to error by setting memory_comsumption very large.
The resulting ACCEL_LOG_FATAL should cause php to die.
The process should die regardless of the log_verbosity_level.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.memory_consumption=999999999
opcache.log_verbosity_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump("Script should fail");
?>
--EXPECTF--

