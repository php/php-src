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

function times23(int $x): int
{
   return $x * 23;
}

class Times27
{
    function __invoke(int $x): int
    {
        return $x * 27;
    }
}


$test = new Test();

$add3 = fn($x) => _add($x, 3);

$res1 = 1
    |> times3(...)
    |> 'times5'
    |> $test->times7(...)
    |> [$test, 'times11']
    |> StaticTest::times13(...)
    |> [StaticTest::class, 'times17']
    |> fn($x) => times23($x)
    |> new Times27()
;

var_dump($res1);
?>
--EXPECT--
int(158513355)
