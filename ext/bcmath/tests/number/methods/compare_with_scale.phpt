--TEST--
BcMath\Number compare() with scale
--EXTENSIONS--
bcmath
--FILE--
<?php

$values2 = [
    [100, 'int'],
    ['100.0000', 'string'],
    [new BcMath\Number('100.0000'), 'object'],
    ['100.0001', 'string'],
    [new BcMath\Number('100.0001'), 'object'],
    ['100.0010', 'string'],
    [new BcMath\Number('100.0010'), 'object'],
    ['100.0100', 'string'],
    [new BcMath\Number('100.0100'), 'object'],
    ['100.0011', 'string'],
    [new BcMath\Number('100.0011'), 'object'],
];

$value1 = new BcMath\Number('100.0011');

$scales = [0, 1, 2, 3, 4, 5];

foreach ($scales as $scale) {
    echo "========== scale is {$scale} ==========\n";
    foreach ($values2 as [$value2, $type2]) {
        echo "with {$type2} {$value2}:\n";
        var_dump($value1->compare($value2, $scale));

        echo "\n";
    }
}
?>
--EXPECT--
========== scale is 0 ==========
with int 100:
int(0)

with string 100.0000:
int(0)

with object 100.0000:
int(0)

with string 100.0001:
int(0)

with object 100.0001:
int(0)

with string 100.0010:
int(0)

with object 100.0010:
int(0)

with string 100.0100:
int(0)

with object 100.0100:
int(0)

with string 100.0011:
int(0)

with object 100.0011:
int(0)

========== scale is 1 ==========
with int 100:
int(0)

with string 100.0000:
int(0)

with object 100.0000:
int(0)

with string 100.0001:
int(0)

with object 100.0001:
int(0)

with string 100.0010:
int(0)

with object 100.0010:
int(0)

with string 100.0100:
int(0)

with object 100.0100:
int(0)

with string 100.0011:
int(0)

with object 100.0011:
int(0)

========== scale is 2 ==========
with int 100:
int(0)

with string 100.0000:
int(0)

with object 100.0000:
int(0)

with string 100.0001:
int(0)

with object 100.0001:
int(0)

with string 100.0010:
int(0)

with object 100.0010:
int(0)

with string 100.0100:
int(-1)

with object 100.0100:
int(-1)

with string 100.0011:
int(0)

with object 100.0011:
int(0)

========== scale is 3 ==========
with int 100:
int(1)

with string 100.0000:
int(1)

with object 100.0000:
int(1)

with string 100.0001:
int(1)

with object 100.0001:
int(1)

with string 100.0010:
int(0)

with object 100.0010:
int(0)

with string 100.0100:
int(-1)

with object 100.0100:
int(-1)

with string 100.0011:
int(0)

with object 100.0011:
int(0)

========== scale is 4 ==========
with int 100:
int(1)

with string 100.0000:
int(1)

with object 100.0000:
int(1)

with string 100.0001:
int(1)

with object 100.0001:
int(1)

with string 100.0010:
int(1)

with object 100.0010:
int(1)

with string 100.0100:
int(-1)

with object 100.0100:
int(-1)

with string 100.0011:
int(0)

with object 100.0011:
int(0)

========== scale is 5 ==========
with int 100:
int(1)

with string 100.0000:
int(1)

with object 100.0000:
int(1)

with string 100.0001:
int(1)

with object 100.0001:
int(1)

with string 100.0010:
int(1)

with object 100.0010:
int(1)

with string 100.0100:
int(-1)

with object 100.0100:
int(-1)

with string 100.0011:
int(0)

with object 100.0011:
int(0)
