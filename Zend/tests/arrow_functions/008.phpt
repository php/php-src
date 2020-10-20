--TEST--
Yield inside arrow functions
--FILE--
<?php

// This doesn't make terribly much sense, but it works...

$fn = fn() => yield 1;
foreach ($fn() as $val) {
    var_dump($val);
}

$fn = fn() { return yield 2; };
foreach ($fn() as $val) {
    var_dump($val);
}

$fn = fn() => yield from [3, 4];
foreach ($fn() as $val) {
    var_dump($val);
}

$fn = fn() { yield from [5, 6]; };
foreach ($fn() as $val) {
    var_dump($val);
}

$fn = fn() => fn() => yield 7;
foreach ($fn()() as $val) {
    var_dump($val);
}

$fn = fn() => fn() { yield 8; };
foreach ($fn()() as $val) {
    var_dump($val);
}

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
