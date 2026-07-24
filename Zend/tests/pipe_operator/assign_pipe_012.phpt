--TEST--
Pipe assign operator sequential usage equivalent to chain
--FILE--
<?php

$s = "  Hello World  ";
$s |>= trim(...);
$s |>= strtolower(...);
$s |>= (fn($s) => str_replace(" ", "-", $s));
var_dump($s);

$s2 = "  Hello World  ";
$s2 |>= trim(...) |> strtolower(...) |> (fn($s) => str_replace(" ", "-", $s));
var_dump($s2);

var_dump($s === $s2);

?>
--EXPECT--
string(11) "hello-world"
string(11) "hello-world"
bool(true)
