--TEST--
BcMath\Number calc methods (add, sub, mul, div, mod, pow) scale arg error
--EXTENSIONS--
bcmath
--FILE--
<?php
$args = [
    [[], 'array'],
    [new stdClass(), 'other object'],
    [0.1, 'float'],
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
            $num->$method(1, $val);
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
    echo "\n";
}
?>
--EXPECTF--
========== add ==========
array:
TypeError: BcMath\Number::add(): Argument #2 ($scale) must be of type ?int, array given
other object:
TypeError: BcMath\Number::add(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== sub ==========
array:
TypeError: BcMath\Number::sub(): Argument #2 ($scale) must be of type ?int, array given
other object:
TypeError: BcMath\Number::sub(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== mul ==========
array:
TypeError: BcMath\Number::mul(): Argument #2 ($scale) must be of type ?int, array given
other object:
TypeError: BcMath\Number::mul(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== div ==========
array:
TypeError: BcMath\Number::div(): Argument #2 ($scale) must be of type ?int, array given
other object:
TypeError: BcMath\Number::div(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== mod ==========
array:
TypeError: BcMath\Number::mod(): Argument #2 ($scale) must be of type ?int, array given
other object:
TypeError: BcMath\Number::mod(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== pow ==========
array:
TypeError: BcMath\Number::pow(): Argument #2 ($scale) must be of type ?int, array given
other object:
TypeError: BcMath\Number::pow(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
