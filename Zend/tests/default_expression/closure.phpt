--TEST--
Tests passing default to closures
--FILE--
<?php

$F = function ($X = 1, $Y = 2) {
    return $X + $Y;
};
var_dump($F(default, $V = default));
var_dump($V);

$F2 = fn ($P = 'Alfa') => $P;
var_dump($F2($V = default));
var_dump($V);
?>
--EXPECT--
int(3)
int(2)
string(4) "Alfa"
string(4) "Alfa"
