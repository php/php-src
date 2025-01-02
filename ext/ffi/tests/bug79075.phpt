--TEST--
Bug #79075 (FFI header parser chokes on comments)
--EXTENSIONS--
ffi
opcache
posix
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows');
if (posix_geteuid() == 0) die('skip Cannot run test as root.');
?>
--INI--
ffi.enable=1
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/bug79075.inc
opcache.file_cache_only=0
--FILE--
<?php
$ffi = FFI::scope("bug79075");
$ffi->printf("Hello World from %s!\n", "PHP");
?>
--EXPECT--
Hello World from PHP!
