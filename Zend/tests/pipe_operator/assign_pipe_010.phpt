--TEST--
Pipe assign operator with mixed callable types
--FILE--
<?php

function double(int $x): int { return $x * 2; }

class Math {
    public function triple(int $x): int { return $x * 3; }
    public static function quadruple(int $x): int { return $x * 4; }
}

$math = new Math();

$v1 = 5;
$v1 |>= double(...);
var_dump($v1);

$v2 = 5;
$v2 |>= $math->triple(...);
var_dump($v2);

$v3 = 5;
$v3 |>= Math::quadruple(...);
var_dump($v3);

class Multiplier {
    public function __construct(private int $factor) {}
    public function __invoke(int $x): int { return $x * $this->factor; }
}

$v4 = 5;
$v4 |>= new Multiplier(5);
var_dump($v4);

$times6 = fn($x) => $x * 6;
$v5 = 5;
$v5 |>= $times6;
var_dump($v5);

?>
--EXPECT--
int(10)
int(15)
int(20)
int(25)
int(30)
