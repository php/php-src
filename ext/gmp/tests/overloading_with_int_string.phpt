--TEST--
GMP operator overloading does support int strings
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);

try {
    var_dump($num + "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> "2");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "44"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "40"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "84"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "21"
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
  string(2) "42"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "2"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "40"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(3) "168"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "10"
}
