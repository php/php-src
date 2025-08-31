--TEST--
BcMath\Number properties unknown
--EXTENSIONS--
bcmath
--FILE--
<?php

$num = new BcMath\Number(1);

var_dump($num->foo);

try {
    $num->foo = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($num->foo);

try {
    $num->bar = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(isset($num->foo));
?>
--EXPECTF--
Warning: Undefined property: BcMath\Number::$foo in %s
NULL
Cannot create dynamic property BcMath\Number::$foo

Warning: Undefined property: BcMath\Number::$foo in %s
NULL
Cannot create dynamic property BcMath\Number::$bar
bool(false)
