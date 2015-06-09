--TEST--
Test that script cached info is correct
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.validate_timestamps=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(opcache_is_script_cached(__FILE__));
var_dump(opcache_is_script_cached("nonexistent.php"));
?>
--EXPECT--
bool(true)
bool(false)
