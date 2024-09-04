--TEST--
BcMath\Number properties write error
--EXTENSIONS--
bcmath
--FILE--
<?php

$num = new BcMath\Number(1);
try {
    $num->value = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num->scale = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Cannot modify readonly property BcMath\Number::$value
Cannot modify readonly property BcMath\Number::$scale
