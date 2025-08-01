--TEST--
Invokable objects may be composed
--FILE--
<?php

class Times {
    public function __construct(private int $x) {}

    public function __invoke(int $y): int
    {
        return $this->x * $y;
    }
}

function double(int $x): int {
    return $x * 2;
}

$cb1 = double(...) + new Times(3);
var_dump($cb1(4));

$cb2 = new Times(3) + double(...);
var_dump($cb2(4));

--EXPECT--
int(24)
int(24)
