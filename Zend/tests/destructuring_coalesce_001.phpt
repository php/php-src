--TEST--
Destructuring operations with coalesce
--FILE--
<?php

[$a ?? 1] = [];
var_dump($a);

["strkey" => $a ?? 2] = [];
var_dump($a);

[, $a ?? 0] = [0, 3];
var_dump($a);

["strkey" => $a ?? 0] = ["strkey" => 4];
var_dump($a);

[$a ?? 5] = "";
var_dump($a);

[$a ?? 6] = [null];
var_dump($a);

[[$a ?? 7]] = [[]];
var_dump($a);

[[$a ?? 8]] = [];
var_dump($a);

[$a ?? 9] = 0;
var_dump($a);

[$a ?? 10] = $undefinedVar;
var_dump($a);

[[$a] ?? [11]] = [];
var_dump($a);

?>
--EXPECTF--
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)

Warning: Undefined array key 0 in %s on line %d
int(8)
int(9)

Warning: Undefined variable $undefinedVar in %s on line %d
int(10)
int(11)
