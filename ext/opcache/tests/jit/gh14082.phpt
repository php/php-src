--TEST--
GH-14082 (Segmentation fault on unknown address 0x600000000018 in ext/opcache/jit/zend_jit.c)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1235
opcache.jit_buffer_size=16M
opcache.preload={PWD}/preload_gh14082.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
Foo::test();
echo "ok\n";
?>
--EXPECT--
int(1)
int(1)
ok
