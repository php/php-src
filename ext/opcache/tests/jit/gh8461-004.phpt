--TEST--
Bug GH-8461 004 (JIT does not account for class re-compile)
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

// Checks that JITed code does not crash in --repeat 2 after the UniqueList
// class changes.

if (!isset(opcache_get_status()['scripts'][__DIR__ . '/gh8461-004.inc'])) {
    $initialRequest = true;
    require __DIR__ . '/gh8461-004.inc';

} else {
    $initialRequest = false;
    $y = 0;

    class UniqueList
    {
        public const A = 1;
        public const B = 1;

        private $foo;

        public function __construct($b)
        {
            global $y;
            $y++;

            $this->foo = self::A + $b;
        }
    }
}

class UniqueListLast extends UniqueList
{
    public function __construct()
    {
        parent::__construct(self::B);
    }
}

for ($i = 0; $i < 10; $i++) {
    new UniqueListLast();
}

var_dump($initialRequest ? $x : $y);
print "OK";
--EXPECT--
int(10)
OK
