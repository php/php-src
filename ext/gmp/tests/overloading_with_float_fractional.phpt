--TEST--
GMP operator overloading support for float with fractional is deprecated
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);

try {
    var_dump($num + 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> 42.5);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "84"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(4) "1764"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(69) "150130937545296572356771972164254457814047970568738777235893533016064"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "42"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "42"
}

Deprecated: Implicit conversion from float 42.5 to int loses precision in %s on line %d
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(15) "184717953466368"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
