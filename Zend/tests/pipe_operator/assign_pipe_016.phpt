--TEST--
Pipe assign operator nested and compound expressions
--FILE--
<?php

function triple(int $x): int { return $x * 3; }
function add_five(int $x): int { return $x + 5; }
function to_string(int $x): string { return "result=$x"; }
function double(int $x): int { return $x * 2; }
function square(int $x): int { return $x * $x; }
function negate(int $x): int { return -$x; }

$e = 4;
$f = ($e |>= triple(...)) |> add_five(...) |> to_string(...);
echo "e=", $e, ", f=", $f, PHP_EOL;

$g = 3;
$h = 10;
$h |>= (function($v) use (&$g) { return $v + ($g |>= double(...)); });
echo "g=", $g, ", h=", $h, PHP_EOL;

$a = 2;
$b = 3;
$c = ($a |>= double(...)) + ($b |>= triple(...));
echo "a=", $a, ", b=", $b, ", c=", $c, PHP_EOL;

$x = 5;
$y = 2;
$y |>= (function($v) use (&$x) { return $v + ($x |>= triple(...) |> add_five(...)); });
echo "x=", $x, ", y=", $y, PHP_EOL;

$p = 2;
$q = 3;
$r = 4;
$r |>= (function($v) use (&$q, &$p) { return $v * ($q |>= (function($w) use (&$p) { return $w + ($p |>= double(...)); })); });
echo "p=", $p, ", q=", $q, ", r=", $r, PHP_EOL;

?>
--EXPECT--
e=12, f=result=17
g=6, h=16
a=4, b=9, c=13
x=20, y=22
p=4, q=7, r=28
