--TEST--
BcMath\Number compare by operator with non-sense
--EXTENSIONS--
bcmath
--FILE--
<?php

$values2 = [
    [null, 'null'],
    ['string', 'string'],
    [new stdClass(), 'object'],
    [[], 'array'],
    [STDERR, 'resource'],
];

$value1 = new BcMath\Number('100.0000');

foreach ($values2 as [$value2, $type2]) {
    echo "========== with {$type2} ==========\n";
    echo "{$value1} >  {$type2}: " . ($value1 > $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} >= {$type2}: " . ($value1 >= $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} == {$type2}: " . ($value1 == $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} <= {$type2}: " . ($value1 <= $value2 ? 'true' : 'false') . "\n";
    echo "{$value1} <  {$type2}: " . ($value1 < $value2 ? 'true' : 'false') . "\n";

    echo "\ninversion\n";
    echo "{$type2} >  {$value1}: " . ($value2 > $value1 ? 'true' : 'false') . "\n";
    echo "{$type2} >= {$value1}: " . ($value2 >= $value1 ? 'true' : 'false') . "\n";
    echo "{$type2} == {$value1}: " . ($value2 == $value1 ? 'true' : 'false') . "\n";
    echo "{$type2} <= {$value1}: " . ($value2 <= $value1 ? 'true' : 'false') . "\n";
    echo "{$type2} <  {$value1}: " . ($value2 < $value1 ? 'true' : 'false') . "\n";

    echo "\n";
}
?>
--EXPECT--
========== with null ==========
100.0000 >  null: true
100.0000 >= null: true
100.0000 == null: false
100.0000 <= null: false
100.0000 <  null: false

inversion
null >  100.0000: false
null >= 100.0000: false
null == 100.0000: false
null <= 100.0000: true
null <  100.0000: true

========== with string ==========
100.0000 >  string: false
100.0000 >= string: false
100.0000 == string: false
100.0000 <= string: false
100.0000 <  string: false

inversion
string >  100.0000: false
string >= 100.0000: false
string == 100.0000: false
string <= 100.0000: false
string <  100.0000: false

========== with object ==========
100.0000 >  object: false
100.0000 >= object: false
100.0000 == object: false
100.0000 <= object: false
100.0000 <  object: false

inversion
object >  100.0000: false
object >= 100.0000: false
object == 100.0000: false
object <= 100.0000: false
object <  100.0000: false

========== with array ==========
100.0000 >  array: false
100.0000 >= array: false
100.0000 == array: false
100.0000 <= array: false
100.0000 <  array: false

inversion
array >  100.0000: false
array >= 100.0000: false
array == 100.0000: false
array <= 100.0000: false
array <  100.0000: false

========== with resource ==========
100.0000 >  resource: false
100.0000 >= resource: false
100.0000 == resource: false
100.0000 <= resource: false
100.0000 <  resource: false

inversion
resource >  100.0000: false
resource >= 100.0000: false
resource == 100.0000: false
resource <= 100.0000: false
resource <  100.0000: false
