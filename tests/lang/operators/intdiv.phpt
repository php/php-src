--TEST--
intdiv functionality
--FILE--
<?php
var_dump(3 %% 2);
var_dump(-3 %% 2);
var_dump(3 %% -2);
var_dump(-3 %% -2);
var_dump(PHP_INT_MAX %% PHP_INT_MAX);
var_dump((-PHP_INT_MAX - 1) %% (-PHP_INT_MAX - 1));
var_dump((-PHP_INT_MAX - 1) %% -1);
var_dump(1 %% 0);

?>
--EXPECTF--
int(1)
int(-1)
int(-1)
int(1)
int(1)
int(1)
int(0)

Warning: Division by zero in %s on line 9
bool(false)
