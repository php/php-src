--TEST--
GMP operator overloading does not support resources
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);

try {
    var_dump($num + STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> STDERR);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Number must be of type GMP|string|int, resource given
TypeError: Number must be of type GMP|string|int, resource given
TypeError: Number must be of type GMP|string|int, resource given
TypeError: Number must be of type GMP|string|int, resource given
TypeError: Number must be of type GMP|string|int, resource given
object(GMP)#3 (1) {
  ["num"]=>
  string(5) "74088"
}
TypeError: Number must be of type GMP|string|int, resource given
TypeError: Number must be of type GMP|string|int, resource given
TypeError: Number must be of type GMP|string|int, resource given
object(GMP)#2 (1) {
  ["num"]=>
  string(3) "336"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "5"
}
