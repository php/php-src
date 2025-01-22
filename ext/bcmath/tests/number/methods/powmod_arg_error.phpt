--TEST--
BcMath\Number powmod arg error
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

echo "========== check 1st arg ==========\n";
foreach ($args as [$val, $type]) {
    echo "{$type}:\n";
    try {
        $num->powmod($val, 1);
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}

echo "\n";

echo "========== check 2nd arg ==========\n";
foreach ($args as [$val, $type]) {
    echo "{$type}:\n";
    try {
        $num->powmod(1, $val);
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}
?>
--EXPECTF--
========== check 1st arg ==========
non number str:
BcMath\Number::powmod(): Argument #1 ($exponent) is not well-formed
array:
BcMath\Number::powmod(): Argument #1 ($exponent) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::powmod(): Argument #1 ($exponent) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
null:

Deprecated: BcMath\Number::powmod(): Passing null to parameter #1 ($exponent) of type BcMath\Number|string|int is deprecated in %s

========== check 2nd arg ==========
non number str:
BcMath\Number::powmod(): Argument #2 ($modulus) is not well-formed
array:
BcMath\Number::powmod(): Argument #2 ($modulus) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::powmod(): Argument #2 ($modulus) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
Modulo by zero
null:

Deprecated: BcMath\Number::powmod(): Passing null to parameter #2 ($modulus) of type BcMath\Number|string|int is deprecated in %s
Modulo by zero
