--TEST--
GMP operator overloading does support float with no fractional
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);

try {
    var_dump($num + 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> 42.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "84"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(4) "1764"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(69) "150130937545296572356771972164254457814047970568738777235893533016064"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "42"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "42"
}
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
