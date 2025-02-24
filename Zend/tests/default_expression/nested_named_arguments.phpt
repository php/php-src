--TEST--
Tests passing default to a named parent argument that is different from the child call's last named argument
--FILE--
<?php

$F = fn ($X = 1, $Y = 2) => $X + $Y;
var_dump($F(X: $F(X: 0, Y: 1) + default));
?>
--EXPECTF--
int(4)
