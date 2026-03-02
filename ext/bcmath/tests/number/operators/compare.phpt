--TEST--
BcMath\Number compare by operator
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
    echo "{$value1} >  {$value2}: " . ($value1 > $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} >= {$value2}: " . ($value1 >= $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} == {$value2}: " . ($value1 == $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} <= {$value2}: " . ($value1 <= $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} <  {$value2}: " . ($value1 < $value2 ? 'true' : 'false') . "\n";

    echo "\ninversion\n";
    echo "{$value2} >  {$value1}: " . ($value2 > $value1 ? 'true' : 'false') . "\n";
    echo "{$value2} >= {$value1}: " . ($value2 >= $value1 ? 'true' : 'false') . "\n";
    echo "{$value2} == {$value1}: " . ($value2 == $value1 ? 'true' : 'false') . "\n";
    echo "{$value2} <= {$value1}: " . ($value2 <= $value1 ? 'true' : 'false') . "\n";
    echo "{$value2} <  {$value1}: " . ($value2 < $value1 ? 'true' : 'false') . "\n";

    echo "\n";
}
?>
--EXPECT--
========== with int 99 ==========
100.0000 >  99: true
100.0000 >= 99: true
100.0000 == 99: false
100.0000 <= 99: false
100.0000 <  99: false

inversion
99 >  100.0000: false
99 >= 100.0000: false
99 == 100.0000: false
99 <= 100.0000: true
99 <  100.0000: true

========== with string 99.9999 ==========
100.0000 >  99.9999: true
100.0000 >= 99.9999: true
100.0000 == 99.9999: false
100.0000 <= 99.9999: false
100.0000 <  99.9999: false

inversion
99.9999 >  100.0000: false
99.9999 >= 100.0000: false
99.9999 == 100.0000: false
99.9999 <= 100.0000: true
99.9999 <  100.0000: true

========== with object 99.9999 ==========
100.0000 >  99.9999: true
100.0000 >= 99.9999: true
100.0000 == 99.9999: false
100.0000 <= 99.9999: false
100.0000 <  99.9999: false

inversion
99.9999 >  100.0000: false
99.9999 >= 100.0000: false
99.9999 == 100.0000: false
99.9999 <= 100.0000: true
99.9999 <  100.0000: true

========== with int 100 ==========
100.0000 >  100: false
100.0000 >= 100: true
100.0000 == 100: true
100.0000 <= 100: true
100.0000 <  100: false

inversion
100 >  100.0000: false
100 >= 100.0000: true
100 == 100.0000: true
100 <= 100.0000: true
100 <  100.0000: false

========== with string 100 ==========
100.0000 >  100: false
100.0000 >= 100: true
100.0000 == 100: true
100.0000 <= 100: true
100.0000 <  100: false

inversion
100 >  100.0000: false
100 >= 100.0000: true
100 == 100.0000: true
100 <= 100.0000: true
100 <  100.0000: false

========== with string 100.0000 ==========
100.0000 >  100.0000: false
100.0000 >= 100.0000: true
100.0000 == 100.0000: true
100.0000 <= 100.0000: true
100.0000 <  100.0000: false

inversion
100.0000 >  100.0000: false
100.0000 >= 100.0000: true
100.0000 == 100.0000: true
100.0000 <= 100.0000: true
100.0000 <  100.0000: false

========== with object 100 ==========
100.0000 >  100: false
100.0000 >= 100: true
100.0000 == 100: true
100.0000 <= 100: true
100.0000 <  100: false

inversion
100 >  100.0000: false
100 >= 100.0000: true
100 == 100.0000: true
100 <= 100.0000: true
100 <  100.0000: false

========== with object 100.0000 ==========
100.0000 >  100.0000: false
100.0000 >= 100.0000: true
100.0000 == 100.0000: true
100.0000 <= 100.0000: true
100.0000 <  100.0000: false

inversion
100.0000 >  100.0000: false
100.0000 >= 100.0000: true
100.0000 == 100.0000: true
100.0000 <= 100.0000: true
100.0000 <  100.0000: false

========== with int 101 ==========
100.0000 >  101: false
100.0000 >= 101: false
100.0000 == 101: false
100.0000 <= 101: true
100.0000 <  101: true

inversion
101 >  100.0000: true
101 >= 100.0000: true
101 == 100.0000: false
101 <= 100.0000: false
101 <  100.0000: false

========== with string 100.00001 ==========
100.0000 >  100.00001: false
100.0000 >= 100.00001: false
100.0000 == 100.00001: false
100.0000 <= 100.00001: true
100.0000 <  100.00001: true

inversion
100.00001 >  100.0000: true
100.00001 >= 100.0000: true
100.00001 == 100.0000: false
100.00001 <= 100.0000: false
100.00001 <  100.0000: false

========== with object 100.00001 ==========
100.0000 >  100.00001: false
100.0000 >= 100.00001: false
100.0000 == 100.00001: false
100.0000 <= 100.00001: true
100.0000 <  100.00001: true

inversion
100.00001 >  100.0000: true
100.00001 >= 100.0000: true
100.00001 == 100.0000: false
100.00001 <= 100.0000: false
100.00001 <  100.0000: false
