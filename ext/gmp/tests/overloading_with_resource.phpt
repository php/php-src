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
TypeError: Unsupported operand types: GMP + resource
TypeError: Unsupported operand types: GMP - resource
TypeError: Unsupported operand types: GMP * resource
TypeError: Unsupported operand types: GMP / resource
TypeError: Unsupported operand types: GMP % resource
object(GMP)#3 (1) {
  ["num"]=>
  string(5) "74088"
}
TypeError: Unsupported operand types: GMP | resource
TypeError: Unsupported operand types: GMP & resource
TypeError: Unsupported operand types: GMP ^ resource
object(GMP)#2 (1) {
  ["num"]=>
  string(3) "336"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "5"
}
