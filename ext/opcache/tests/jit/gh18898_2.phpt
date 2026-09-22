--TEST--
GH-18898 (SEGV zend_jit_op_array_hot with property hooks and preloading) - jit 1233
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1233
opcache.jit_buffer_size=16M
opcache.preload={PWD}/../gh18534_preload.inc
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php
$test = new DummyModel;
var_dump($test->dummyProperty2);
echo "ok";
?>
--EXPECT--
NULL
ok
