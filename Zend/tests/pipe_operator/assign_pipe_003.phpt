--TEST--
Pipe assign operator chain support (multiple |> on RHS)
--FILE--
<?php

$val = 10;
$val |>= (fn($v) => $v * 2) |> (fn($v) => $v + 3) |> (fn($v) => $v * $v);
var_dump($val);

$data = "  the quick brown FOX  ";
$data |>= trim(...)
    |> strtolower(...)
    |> (fn($s) => explode(" ", $s))
    |> (fn($a) => array_map(ucfirst(...), $a))
    |> (fn($a) => implode("-", $a));
var_dump($data);

?>
--EXPECT--
int(529)
string(19) "The-Quick-Brown-Fox"
