--TEST--
GH-24088 (Consumers outside range propagation must not trust ranges inferred for other types)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function divide(int|bool $divisor) {
    if ($divisor <= -1) {
        1 / $divisor;
        return 'no exception';
    }
}

function modulo(int|bool $divisor) {
    if ($divisor <= -1) {
        1 % $divisor;
        return 'no exception';
    }
}

function constrained($value) {
    if ($value >= 6 && $value <= 6) {
        return $value;
    }
    return 6;
}

function callerModulo($value) {
    return constrained($value) % 1000;
}

function nullBound(int $integer, ?int $bound) {
    if ($integer > $bound) {
        return $integer - 1;
    }
    return 0;
}

foreach (['divide', 'modulo'] as $function) {
    try {
        $result = $function(false);
        echo $function, ': ', $result, "\n";
    } catch (DivisionByZeroError $e) {
        echo $function, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}
echo 'call result: ', callerModulo(true), "\n";
echo 'null bound: ', var_export(nullBound(PHP_INT_MIN, null), true), "\n";
?>
--EXPECT--
divide: DivisionByZeroError: Division by zero
modulo: DivisionByZeroError: Modulo by zero
call result: 1
null bound: -9.223372036854776E+18
