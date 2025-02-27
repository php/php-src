<?php

$x = 0;

class UniqueList
{
    const A = 1;

    public static function foo()
    {
        global $x;
        $x += self::A;
    }
}
