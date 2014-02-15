--TEST--
Test that script cached info is correct
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

opcache_compile_file("files/ScriptToCache.php");
var_dump(opcache_script_cached("files/ScriptToCache.php"));
var_dump(opcache_script_cached("nonexistent.php"));

?>
--EXPECT--
bool(true)
bool(false)
