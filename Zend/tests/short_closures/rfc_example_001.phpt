--TEST--
Short closures: RFC example
--FILE--
<?php

$a = 1;
$b = 2;
$c = 3;
$ret = match ($a) {
    1, 3, 5 => (fn() {
        $val = $a * $b;
        return $val * $c;
    })(),
    2, 4, 6 => (fn() {
        $val = $a + $b;
        return $val + $c;
    })(),
};
var_dump($ret);
--EXPECT--
int(6)
