--TEST--
GMP operator overloading does not support stringable objects
--EXTENSIONS--
gmp
--FILE--
<?php

class T {
    public function __toString() {
        return "42";
    }
}

$num = gmp_init(42);
$o = new T();

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
--EXPECTF--
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Unsupported operand types: GMP ** T
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Unsupported operand types: GMP << T
TypeError: Unsupported operand types: GMP >> T
