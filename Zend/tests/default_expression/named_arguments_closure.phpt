--TEST--
Tests passing default as a named argument to a closure
--FILE--
<?php

$F = fn ($V = 1, $default = 2) => $V + $default;
var_dump($F(default: default + 1));
?>
--EXPECT--
int(4)
