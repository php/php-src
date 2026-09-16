--TEST--
BcMath\Number properties write error
--EXTENSIONS--
bcmath
--FILE--
<?php

$num = new BcMath\Number(1);
try {
    $num->value = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $num->scale = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot modify readonly property BcMath\Number::$value
Error: Cannot modify readonly property BcMath\Number::$scale
