--TEST--
Bug GH-8461 007 (JIT does not account for function re-compile)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip Windows does not support preloading");
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1255
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
opcache.preload={PWD}/gh8461-007.inc
--FILE--
<?php

for ($i = 0; $i < 100; $i++) {
    UniqueListLast::bar();
}

for ($i = 0; $i < 100; $i++) {
    new UniqueListLast();
}

for ($i = 0; $i < 10; $i++) {
    test();
}

print "OK";
?>
--EXPECT--
OK
