--TEST--
BcMath\Number calc methods (add, sub, mul, div, mod, pow) num arg error
--EXTENSIONS--
bcmath
--FILE--
<?php
$args = [
    [[], 'array'],
    [new stdClass(), 'other object'],
    [0.1, 'float'],
    [null, 'null'],
];

$methods = [
    'add',
    'sub',
    'mul',
    'div',
    'mod',
    'pow',
];

$num = new BcMath\Number('100.0000');
foreach ($methods as $method) {
    echo "========== {$method} ==========\n";
    foreach ($args as [$val, $type]) {
        echo "{$type}:\n";
        try {
            $num->$method($val);
        } catch (Error $e) {
            echo $e->getMessage() . "\n";
        }
    }
    echo "\n";
}
?>
--EXPECTF--
========== add ==========
array:
BcMath\Number::add(): Argument #1 ($num) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::add(): Argument #1 ($num) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
null:

Deprecated: BcMath\Number::add(): Passing null to parameter #1 ($num) of type BcMath\Number|string|int is deprecated in %s

========== sub ==========
array:
BcMath\Number::sub(): Argument #1 ($num) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::sub(): Argument #1 ($num) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
null:

Deprecated: BcMath\Number::sub(): Passing null to parameter #1 ($num) of type BcMath\Number|string|int is deprecated in %s

========== mul ==========
array:
BcMath\Number::mul(): Argument #1 ($num) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::mul(): Argument #1 ($num) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
null:

Deprecated: BcMath\Number::mul(): Passing null to parameter #1 ($num) of type BcMath\Number|string|int is deprecated in %s

========== div ==========
array:
BcMath\Number::div(): Argument #1 ($num) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::div(): Argument #1 ($num) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
Division by zero
null:

Deprecated: BcMath\Number::div(): Passing null to parameter #1 ($num) of type BcMath\Number|string|int is deprecated in %s
Division by zero

========== mod ==========
array:
BcMath\Number::mod(): Argument #1 ($num) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::mod(): Argument #1 ($num) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
Modulo by zero
null:

Deprecated: BcMath\Number::mod(): Passing null to parameter #1 ($num) of type BcMath\Number|string|int is deprecated in %s
Modulo by zero

========== pow ==========
array:
BcMath\Number::pow(): Argument #1 ($exponent) must be of type int, string, or BcMath\Number, array given
other object:
BcMath\Number::pow(): Argument #1 ($exponent) must be of type int, string, or BcMath\Number, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
null:

Deprecated: BcMath\Number::pow(): Passing null to parameter #1 ($exponent) of type BcMath\Number|string|int is deprecated in %s
