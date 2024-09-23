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

Warning: Object of class T could not be converted to int in %s on line %d
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "42"
}
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given
TypeError: Number must be of type GMP|string|int, T given

Warning: Object of class T could not be converted to int in %s on line %d
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "84"
}

Warning: Object of class T could not be converted to int in %s on line %d
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "21"
}
