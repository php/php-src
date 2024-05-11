--TEST--
round() with modes PHP_ROUND_CEILING and PHP_ROUND_FLOOR
--FILE--
<?php
$modes = [
    PHP_ROUND_CEILING, PHP_ROUND_FLOOR,
];

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

echo "mode PHP_ROUND_CEILING\n";
foreach($numbers as $number) {
    var_dump(ceil($number) === round($number, 0, PHP_ROUND_CEILING));
}

echo "\n";
echo "mode PHP_ROUND_FLOOR\n";
foreach($numbers as $number) {
    var_dump(floor($number) === round($number, 0, PHP_ROUND_FLOOR));
}

?>
--EXPECT--
mode PHP_ROUND_CEILING
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

mode PHP_ROUND_FLOOR
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
