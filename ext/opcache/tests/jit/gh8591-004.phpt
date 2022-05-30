--TEST--
Bug GH-8591 004 (JIT does not account for class re-compile)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=1255
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
--FILE--
<?php

// Checks that JITed code does not crash in --repeat 2 after the ModelTrait
// trait is recompiled and Model is re-linked.

require __DIR__ . '/gh8591-004.inc';

class Model
{
    use ModelTrait;

    protected static int $field = 1;

    public function __construct()
    {
        for ($i = 0; $i < 10; $i++) {
            $this->cast();
        }
    }

    private function cast()
    {
        global $x;
        $x = static::$field;
    }
}

new Model();

// mark the file as changed (important)
touch(__DIR__ . '/gh8591-004.inc');

var_dump($x);

print "OK";
--EXPECT--
int(1)
OK
