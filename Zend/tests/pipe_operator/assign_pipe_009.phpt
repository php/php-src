--TEST--
Pipe assign operator expression result and precedence
--FILE--
<?php

function triple(int $x): int { return $x * 3; }
function add_five(int $x): int { return $x + 5; }
function to_string(int $x): string { return "value:$x"; }

$x = 5;
$result = ($x |>= triple(...));
var_dump($x, $result);

$e = 4;
$f = ($e |>= triple(...)) |> add_five(...) |> to_string(...);
var_dump($e, $f);

// Right-associativity
// Parses as: $foo |>= ($bar |>= x(...))
// 1. $bar |>= x(...) calls x(0), which prints int(0) and returns a Closure
// 2. $bar is assigned the Closure
// 3. $foo |>= <Closure> calls Closure(1) = 2
// 4. $foo is assigned 2
function x($bar) {
    var_dump($bar);
    return function ($foo) { return $foo + 1; };
}
$bar = "bar";
$foo = 1;
$foo |>= $bar |>= x(...);
var_dump($bar instanceof Closure, $foo);

$a = 10;
$b = 5;
$a += $b |>= triple(...);
var_dump($a, $b);

$c = 4;
$d = ($c |>= (fn($v) => $v * 2)) |> triple(...);
var_dump($c, $d);

?>
--EXPECT--
int(15)
int(15)
int(12)
string(8) "value:17"
string(3) "bar"
bool(true)
int(2)
int(25)
int(15)
int(8)
int(24)
