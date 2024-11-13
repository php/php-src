--TEST--
GMP operator overloading does not support non-stringable objects
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);
$o = new stdClass();

try {
    var_dump($num + $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Unsupported operand types: GMP ** stdClass
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Number must be of type GMP|string|int, stdClass given
TypeError: Unsupported operand types: GMP << stdClass
TypeError: Unsupported operand types: GMP >> stdClass
