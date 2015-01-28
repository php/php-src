--TEST--
Conversion of special float values to integers
--FILE--
<?php
$values = [
    INF,
    -INF,
    NAN,
    -NAN,
    0,
    -0
];
foreach ($values as $value) {
    var_dump($value);
    var_dump((int)$value);
    echo PHP_EOL;
}
?>
--EXPECT--
float(INF)
int(0)

float(-INF)
int(0)

float(NAN)
int(0)

float(NAN)
int(0)

int(0)
int(0)

int(0)
int(0)

