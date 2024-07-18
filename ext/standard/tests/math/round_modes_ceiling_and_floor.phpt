--TEST--
round() with modes PositiveInfinity and NegativeInfinity
--FILE--
<?php

$numbers = [
    2.5,
    -2.5,
    3.5,
    -3.5,
    7,
    -7,
    0.61,
    0.69,
    0,
    -0,
    1.9999,
    -1.9999,
    0.0001,
    -0.0001,
];

echo "mode PositiveInfinity\n";
foreach($numbers as $number) {
    var_dump(ceil($number) === round($number, 0, RoundingMode::PositiveInfinity));
}

echo "\n";
echo "mode NegativeInfinity\n";
foreach($numbers as $number) {
    var_dump(floor($number) === round($number, 0, RoundingMode::NegativeInfinity));
}

?>
--EXPECT--
mode PositiveInfinity
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

mode NegativeInfinity
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
