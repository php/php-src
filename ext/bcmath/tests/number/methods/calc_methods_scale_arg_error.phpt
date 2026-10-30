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
            echo $e->getMessage() . "\n";
        }
    }
    echo "\n";
}
?>
--EXPECTF--
========== add ==========
array:
BcMath\Number::add(): Argument #2 ($scale) must be of type ?int, array given
other object:
BcMath\Number::add(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== sub ==========
array:
BcMath\Number::sub(): Argument #2 ($scale) must be of type ?int, array given
other object:
BcMath\Number::sub(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== mul ==========
array:
BcMath\Number::mul(): Argument #2 ($scale) must be of type ?int, array given
other object:
BcMath\Number::mul(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== div ==========
array:
BcMath\Number::div(): Argument #2 ($scale) must be of type ?int, array given
other object:
BcMath\Number::div(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== mod ==========
array:
BcMath\Number::mod(): Argument #2 ($scale) must be of type ?int, array given
other object:
BcMath\Number::mod(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s

========== pow ==========
array:
BcMath\Number::pow(): Argument #2 ($scale) must be of type ?int, array given
other object:
BcMath\Number::pow(): Argument #2 ($scale) must be of type ?int, stdClass given
float:

Deprecated: Implicit conversion from float 0.1 to int loses precision in %s
