--TEST--
GH-18639 (Internal class aliases can break preloading + JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1255
opcache.jit_buffer_size=16M
opcache.preload={PWD}/preload_gh18567.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
echo "ok\n";
?>
--EXPECT--
ok
