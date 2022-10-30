--TEST--
FFI 300: FFI preloading
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available'); ?>
<?php if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows'); ?>
<?php if (!extension_loaded('posix')) die('skip POSIX extension not loaded'); ?>
<?php if (posix_geteuid() == 0) die('skip Cannot run test as root.'); ?>
--INI--
ffi.enable=1
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload.inc
opcache.file_cache_only=0
--FILE--
<?php
$ffi = FFI::scope("TEST_300");
$ffi->printf("Hello World from %s!\n", "PHP");
?>
--EXPECT--
Hello World from PHP!
