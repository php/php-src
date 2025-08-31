--TEST--
BcMath\Number properties read
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    '0',
    '0.0',
    '2',
    '1234',
    '12.0004',
    '0.1230',
    1,
    12345,
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);
    echo "value: {$num->value}\n";
    echo "scale: {$num->scale}\n";
    echo "\n";
}
?>
--EXPECT--
value: 0
scale: 0

value: 0.0
scale: 1

value: 2
scale: 0

value: 1234
scale: 0

value: 12.0004
scale: 4

value: 0.1230
scale: 4

value: 1
scale: 0

value: 12345
scale: 0
