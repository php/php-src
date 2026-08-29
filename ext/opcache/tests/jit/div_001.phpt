--TEST--
JIT DIV: floating point specializations and division by zero
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
--EXTENSIONS--
opcache
--FILE--
<?php
function divDoubleDouble(float $left, float $right): float {
    return $left / $right;
}

function divDoubleLong(float $left, int $right): float {
    return $left / $right;
}

function divLongDouble(int $left, float $right): float {
    return $left / $right;
}

for ($i = 0; $i < 10_000; $i++) {
    divDoubleDouble(9.0, 2.0);
    divDoubleLong(9.0, 2);
    divLongDouble(9, 2.0);
}

var_dump(divDoubleDouble(9.0, 2.0));
var_dump(divDoubleLong(9.0, 2));
var_dump(divLongDouble(9, 2.0));
var_dump(is_nan(divDoubleDouble(NAN, 2.0)));
var_dump(is_infinite(divDoubleDouble(INF, 2.0)));

foreach ([0.0, -0.0] as $zero) {
    try {
        divDoubleDouble(1.0, $zero);
    } catch (DivisionByZeroError $e) {
        echo $e->getMessage(), "\n";
    }
}

try {
    divDoubleLong(1.0, 0);
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}

try {
    divLongDouble(1, 0.0);
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
float(4.5)
float(4.5)
float(4.5)
bool(true)
bool(true)
Division by zero
Division by zero
Division by zero
Division by zero
