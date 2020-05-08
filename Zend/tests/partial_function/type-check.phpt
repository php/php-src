--TEST--
Ensure reflected types inherit.
--FILE--
<?php

function volume(int $x, int $y, int $z): int {
    return $x * $y * $z;
}

$p1 = volume(3, 5, ?);

$r = new ReflectionFunction($p1);

print $r->getNumberOfParameters();

$params = $r->getParameters();
print $params[0]->getType();

print $r->getReturnType();

--EXPECTF--
1
int
int
