--TEST--
Test array_reduce() function with callback key
--FILE--
<?php
var_dump(array_reduce(range(3, 5), fn ($acc, $v, $k) => $acc += $k));
var_dump(array_reduce(array_flip(range(3, 5)), fn ($acc, $v, $k) => $acc += $k));
var_dump(array_reduce(range(3, 5), fn ($acc, $v, $k) => $acc += $v + $k));
var_dump(array_reduce(array_flip(['Alfa', 'Bravo']), fn ($acc, $v, $k) => $acc .= $k));
?>
--EXPECT--
int(3)
int(12)
int(15)
string(9) "AlfaBravo"
