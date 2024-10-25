--TEST--
GMP operator overloading does not support float strings
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);

try {
    var_dump($num + "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> "2.0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Number is not an integer string
ValueError: Number is not an integer string
ValueError: Number is not an integer string
ValueError: Number is not an integer string
ValueError: Number is not an integer string
object(GMP)#3 (1) {
  ["num"]=>
  string(4) "1764"
}
ValueError: Number is not an integer string
ValueError: Number is not an integer string
ValueError: Number is not an integer string
object(GMP)#2 (1) {
  ["num"]=>
  string(3) "168"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(2) "10"
}
