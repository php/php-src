<?php

$x = 0;

class UniqueList
{
    public const A = 1;
    public const B = 1;

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

function test()
{
        global $x;
        $x += 1;//test2();
}
