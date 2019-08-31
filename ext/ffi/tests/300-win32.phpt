--TEST--
FFI 300: FFI preloading on Windows
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available'); ?>
<?php if (substr(PHP_OS, 0, 3) != 'WIN') die('skip for Windows only'); ?>
--INI--
ffi.enable=1
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
--FILE--
<?php
$ffi = FFI::scope("TEST_300_WIN32");
$ffi->php_printf("Hello World from %s!\n", "PHP");
?>
--CLEAN--
<?php
	$fn = __DIR__ . "/300-win32.h";
	unlink($fn);
?>
--EXPECT--
Hello World from PHP!
