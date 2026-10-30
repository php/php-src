--TEST--
BcMath\Number compare()
--EXTENSIONS--
bcmath
--FILE--
<?php

$values2 = [
    [99, 'int'],
    ['99.9999', 'string'],
    [new BcMath\Number('99.9999'), 'object'],
    [100, 'int'],
    ['100', 'string'],
    ['100.0000', 'string'],
    [new BcMath\Number(100), 'object'],
    [new BcMath\Number('100.0000'), 'object'],
    [101, 'int'],
    ['100.00001', 'string'],
    [new BcMath\Number('100.00001'), 'object'],
];

$value1 = new BcMath\Number('100.0000');

foreach ($values2 as [$value2, $type2]) {
    echo "========== with {$type2} {$value2} ==========\n";
    var_dump($value1->compare($value2));

    echo "\n";
}
?>
--EXPECT--
========== with int 99 ==========
int(1)

========== with string 99.9999 ==========
int(1)

========== with object 99.9999 ==========
int(1)

========== with int 100 ==========
int(0)

========== with string 100 ==========
int(0)

========== with string 100.0000 ==========
int(0)

========== with object 100 ==========
int(0)

========== with object 100.0000 ==========
int(0)

========== with int 101 ==========
int(-1)

========== with string 100.00001 ==========
int(-1)

========== with object 100.00001 ==========
int(-1)
