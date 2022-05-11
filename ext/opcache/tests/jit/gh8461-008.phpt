--TEST--
Bug GH-8461 008 (JIT does not account for function re-compile)
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

$x = 0;

class UniqueList
{
    const A = 1;
    const B = 1;

    private $foo;

    public function __construct($b)
    {
        global $x;
        $x++;

        $this->foo = self::A + $b;
    }

    public static function foo()
    {
        global $x;
        $x += self::A;
    }
}

class UniqueListLast extends UniqueList
{
    public function __construct()
    {
        parent::__construct(self::B);
    }

    public static function bar() {
        parent::foo();
    }
}

function test() {
        global $x;
        $x += 1;
}

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
--EXPECT--
OK
