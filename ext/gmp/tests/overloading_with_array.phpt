--TEST--
GMP operator overloading does not support []
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);

try {
    var_dump($num + []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Number must be of type GMP|string|int, array given
TypeError: Number must be of type GMP|string|int, array given
TypeError: Number must be of type GMP|string|int, array given
TypeError: Number must be of type GMP|string|int, array given
TypeError: Number must be of type GMP|string|int, array given
object(GMP)#3 (1) {
  ["num"]=>
  string(1) "1"
}
TypeError: Number must be of type GMP|string|int, array given
TypeError: Number must be of type GMP|string|int, array given
TypeError: Number must be of type GMP|string|int, array given
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "42"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "42"
}
