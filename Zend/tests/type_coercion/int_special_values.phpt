--TEST--
Conversion of special float values to int
--FILE--
<?php
$values = [
    0.0,
    INF,
    -INF,
    1 / INF,
    -1 / INF, // Negative zero,
    NAN
];

foreach($values as $value) {
    var_dump($value);
    var_dump((int)$value);
    echo PHP_EOL;
}
?>
--EXPECTF--
float(0)
int(0)

float(INF)

Warning: non-representable float INF was cast to int in %s on line %d
int(0)

float(-INF)

Warning: non-representable float -INF was cast to int in %s on line %d
int(0)

float(0)
int(0)

float(-0)
int(0)

float(NAN)

Warning: non-representable float NAN was cast to int in %s on line %d
int(0)
