--TEST--
Pipe operator handles all callable styles
--FILE--
<?php

function times3(int $x): int
{
    return $x * 3;
}

function times5(int $x): int
{
    return $x * 5;
}

class Test
{
    public function times7(int $x): int
    {
        return $x * 7;
    }

    public function times11(int $x): int
    {
        return $x * 11;
    }
}

class StaticTest
{
    public static function times13(int $x): int
    {
        return $x * 13;
    }

    public static function times17(int $x): int
    {
        return $x * 17;
    }
}

function times19(int $x): int
{
   return $x * 19;
}

class Times23
{
    function __invoke(int $x): int
    {
        return $x * 23;
    }
}

$times29 = function(int $x): int {
    return $x * 29;
};

function times2(int $x): int {
    return $x * 2;
};

$test = new Test();

$res1 = 1
    |> times3(...)
    |> 'times5'
    |> $test->times7(...)
    |> [$test, 'times11']
    |> StaticTest::times13(...)
    |> [StaticTest::class, 'times17']
    |> new Times23()
    |> $times29
    |> (fn($x) => times2($x))
;

var_dump($res1);
?>
--EXPECT--
int(340510170)
