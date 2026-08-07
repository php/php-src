--TEST--
BcMath\Number pow: negative power of zero by operator
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [0, '0'];

$exponents = [
    [-3, 'int'],
    ['-2', 'string'],
    [new BcMath\Number('-2'), 'object'],
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);

    foreach ($exponents as [$exponent, $type]) {
        echo "{$value} ** {$exponent}: {$type}\n";
        try {
            $num ** $exponent;
        } catch (Error $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}
?>
--EXPECT--
0 ** -3: int
DivisionByZeroError: Negative power of zero
0 ** -2: string
DivisionByZeroError: Negative power of zero
0 ** -2: object
DivisionByZeroError: Negative power of zero
0 ** -3: int
DivisionByZeroError: Negative power of zero
0 ** -2: string
DivisionByZeroError: Negative power of zero
0 ** -2: object
DivisionByZeroError: Negative power of zero
