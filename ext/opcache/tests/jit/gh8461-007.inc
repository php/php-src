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
