--TEST--
Tests passing default to a named parent argument whilst calling child arguments positionally
--FILE--
<?php

$F = fn ($X = 1, $Y = 2) => $X + $Y;
var_dump($F(X: $F(0, 1) + default));
?>
--EXPECTF--
int(4)
