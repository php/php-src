--TEST--
Bug GH-8591 001 (JIT does not account for class re-compile)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1255
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
opcache.preload={PWD}/gh8591-006.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

class Model extends AbstractModel
{
    public function __construct()
    {
        for ($i = 0; $i < 10; $i++) {
            $this->cast();
        }
    }
}

new Model();

var_dump($x);

print "OK";
?>
--EXPECT--
int(1)
OK
