--TEST--
Pipe assign operator with userland function chains
--FILE--
<?php

function double(int $x): int { return $x * 2; }
function increment(int $x): int { return $x + 1; }
function square(int $x): int { return $x * $x; }

$a = 5;
$a |>= double(...);
var_dump($a);

$b = 3;
$b |>= double(...) |> increment(...) |> square(...);
var_dump($b);

?>
--EXPECT--
int(10)
int(49)
