--TEST--
overflow check for _php_math_basetozval
--INI--
precision=14
--FILE--
<?php

var_dump(hexdec("012345"));
var_dump(hexdec("12345"));

try {
    var_dump(hexdec("q12345"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    var_dump(hexdec("12345+?!"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    var_dump(hexdec("12345q"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

try {
    var_dump(hexdec("11111111111111111z"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}

var_dump(hexdec("12345678901234567"));
var_dump(hexdec("17fffffffffffffff"));

?>
--EXPECTF--
int(74565)
int(74565)
Exception: Invalid characters passed for attempted conversion
Exception: Invalid characters passed for attempted conversion
Exception: Invalid characters passed for attempted conversion
Exception: Invalid characters passed for attempted conversion

Notice: Input number is larger than PHP_INT_MAX, precision has been lost in conversion in %s on line %d
float(2.0988295476557332E+19)

Notice: Input number is larger than PHP_INT_MAX, precision has been lost in conversion in %s on line %d
float(2.7670116110564327E+19)
