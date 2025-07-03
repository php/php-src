--TEST--
Test ceil() - basic function test for ceil()
--INI--
precision=14
--FILE--
<?php

$values = [
    0,
    -0,
    0.5,
    -0.5,
    1,
    -1,
    1.5,
    -1.5,
    2.6,
    -2.6,
    037,
    0x5F,
    "10.5",
    "-10.5",
    "3.95E3",
    "-3.95E3",
    "039",
];

foreach ($values as $value) {
    echo "ceil($value): ";
    $res = ceil($value);
    var_dump($res);
}

?>
--EXPECT--
ceil(0): float(0)
ceil(0): float(0)
ceil(0.5): float(1)
ceil(-0.5): float(-0)
ceil(1): float(1)
ceil(-1): float(-1)
ceil(1.5): float(2)
ceil(-1.5): float(-1)
ceil(2.6): float(3)
ceil(-2.6): float(-2)
ceil(31): float(31)
ceil(95): float(95)
ceil(10.5): float(11)
ceil(-10.5): float(-10)
ceil(3.95E3): float(3950)
ceil(-3.95E3): float(-3950)
ceil(039): float(39)
