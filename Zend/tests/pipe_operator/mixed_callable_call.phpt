--TEST--
Pipe operator handles all callable styles
--FILE--
<?php

function _add(int $x, int $y): int {
    return $x + $y;
}

function _area(int $x, int $y): int {
    return $x * $y;
}

class _Test
{
    public function message(string $which): string
    {
        if ($which == 1) {
            return "Hello";
        }
        else if ($which == 2) {
            return "Goodbye";
        }
        else {
            return "World";
        }
    }
}

class StaticTest {
    public static function oneMore(int $x): int {
        return $x + 1;
    }
}

function _double(int $x): int {
    return $x * 2;
}

function multiplier(int $x): \Closure
{
    return fn($y) => $x * $y;
}

$test = new _Test();

$add3 = fn($x) => _add($x, 3);

$res1 = 2
    |> [$test, 'message']
    |> 'strlen'
    |> $add3
    |> fn($x) => _area($x, 2)
    |> _double(...)
    |> multiplier(3)
    |> StaticTest::oneMore(...)
;

var_dump($res1);
?>
--EXPECT--
int(121)
