--TEST--
BcMath\Number properties unset
--EXTENSIONS--
bcmath
--FILE--
<?php

$num = new BcMath\Number(1);
try {
    unset($num->value);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    unset($num->scale);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Cannot unset readonly property BcMath\Number::$value
Cannot unset readonly property BcMath\Number::$scale
