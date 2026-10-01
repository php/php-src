--TEST--
BcMath\Number compare() arg error
--EXTENSIONS--
bcmath
--FILE--
<?php
$args = [
    ['a', 'non number str'],
    [[], 'array'],
    [new stdClass(), 'other object'],
    [0.1, 'float'],
    [null, 'null'],
];

$num = new BcMath\Number('100.0000');
foreach ($args as [$val, $type]) {
    echo "{$type}:\n";
    try {
        $num->compare($val);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    echo "\n";
}
?>
--EXPECTF--
non number str:
ValueError: BcMath\Number::compare(): Argument #1 ($num) is not well-formed

array:
TypeError: BcMath\Number::compare(): Argument #1 ($num) must be of type int, string, or BcMath\Number, array given

other object:
TypeError: BcMath\Number::compare(): Argument #1 ($num) must be of type int, string, or BcMath\Number, stdClass given

float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

null:

Deprecated: BcMath\Number::compare(): Passing null to parameter #1 ($num) of type BcMath\Number|string|int is deprecated in %s
