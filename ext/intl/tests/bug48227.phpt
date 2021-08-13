--TEST--
Bug #48227 (NumberFormatter::format leaks memory)
--EXTENSIONS--
intl
--FILE--
<?php

$x = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
foreach (['', 1, NULL, $x] as $value) {
    try {
        var_dump($x->format($value));
    } catch (TypeError $ex) {
        echo $ex->getMessage(), PHP_EOL;
    }
}

?>
--EXPECTF--
NumberFormatter::format(): Argument #1 ($num) must be of type int|float, string given
string(1) "1"

Deprecated: NumberFormatter::format(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
string(1) "0"
NumberFormatter::format(): Argument #1 ($num) must be of type int|float, NumberFormatter given
