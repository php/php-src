--TEST--
BcMath\Number div(), mod(), divmod() by zero
--EXTENSIONS--
bcmath
--FILE--
<?php

$values1 = ['100', '-100'];

$values2 = [
    [0, 'int'],
    [-0, 'int'],
    ['0', 'string'],
    ['-0', 'string'],
    ['0.000', 'string'],
    ['-0.000', 'string'],
    [new BcMath\Number('0'), 'object'],
    [new BcMath\Number('-0'), 'object'],
    [new BcMath\Number('0.000'), 'object'],
    [new BcMath\Number('-0.000'), 'object'],
];

foreach ($values1 as $value1) {
    $num = new BcMath\Number($value1);

    foreach ($values2 as [$value2, $type]) {
        echo "{$value1} and {$value2}: {$type}\n";
        echo "div: ";
        try {
            $num->div($value2);
            echo "NG\n";
        } catch (Error $e) {
            echo $e->getMessage() === 'Division by zero' ? 'OK' :'NG';
            echo "\n";
        }

        echo "mod: ";
        try {
            $num->mod($value2);
            echo "NG\n";
        } catch (Error $e) {
            echo $e->getMessage() === 'Modulo by zero' ? 'OK' :'NG';
            echo "\n";
        }

        echo "divmod: ";
        try {
            $num->divmod($value2);
            echo "NG\n";
        } catch (Error $e) {
            echo $e->getMessage() === 'Division by zero' ? 'OK' :'NG';
            echo "\n";
        }
        echo "\n";
    }
}
?>
--EXPECT--
100 and 0: int
div: OK
mod: OK
divmod: OK

100 and 0: int
div: OK
mod: OK
divmod: OK

100 and 0: string
div: OK
mod: OK
divmod: OK

100 and -0: string
div: OK
mod: OK
divmod: OK

100 and 0.000: string
div: OK
mod: OK
divmod: OK

100 and -0.000: string
div: OK
mod: OK
divmod: OK

100 and 0: object
div: OK
mod: OK
divmod: OK

100 and 0: object
div: OK
mod: OK
divmod: OK

100 and 0.000: object
div: OK
mod: OK
divmod: OK

100 and 0.000: object
div: OK
mod: OK
divmod: OK

-100 and 0: int
div: OK
mod: OK
divmod: OK

-100 and 0: int
div: OK
mod: OK
divmod: OK

-100 and 0: string
div: OK
mod: OK
divmod: OK

-100 and -0: string
div: OK
mod: OK
divmod: OK

-100 and 0.000: string
div: OK
mod: OK
divmod: OK

-100 and -0.000: string
div: OK
mod: OK
divmod: OK

-100 and 0: object
div: OK
mod: OK
divmod: OK

-100 and 0: object
div: OK
mod: OK
divmod: OK

-100 and 0.000: object
div: OK
mod: OK
divmod: OK

-100 and 0.000: object
div: OK
mod: OK
divmod: OK
