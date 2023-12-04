--TEST--
Bug #48227 (NumberFormatter::format leaks memory)
--EXTENSIONS--
intl
--FILE--
<?php

$testcases = ['', 1, NULL, true, false, [], (object)[]];

echo "OOP\n";
$x = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
foreach (array_merge($testcases, [$x]) as $value) {
    try {
        var_dump($x->format($value));
    } catch (TypeError $ex) {
        echo $ex->getMessage(), PHP_EOL;
    }
}

echo "\nProcedural\n";
$x = numfmt_create('en_US', NumberFormatter::DECIMAL);
foreach (array_merge($testcases, [$x]) as $value) {
    try {
        var_dump(numfmt_format($x, $value));
    } catch (TypeError $ex) {
        echo $ex->getMessage(), PHP_EOL;
    }
}

?>
--EXPECTF--
OOP
bool(false)
string(1) "1"

Deprecated: NumberFormatter::format(): Passing null to parameter #1 ($num) of type string|int|float is deprecated in %s on line %d
string(1) "0"
string(1) "1"
string(1) "0"
NumberFormatter::format(): Argument #1 ($num) must be of type string|int|float, array given
NumberFormatter::format(): Argument #1 ($num) must be of type string|int|float, stdClass given
NumberFormatter::format(): Argument #1 ($num) must be of type string|int|float, NumberFormatter given

Non-OOP
bool(false)
string(1) "1"

Deprecated: numfmt_format(): Passing null to parameter #2 ($num) of type string|int|float is deprecated in %s on line %d
string(1) "0"
string(1) "1"
string(1) "0"
numfmt_format(): Argument #2 ($num) must be of type string|int|float, array given
numfmt_format(): Argument #2 ($num) must be of type string|int|float, stdClass given
numfmt_format(): Argument #2 ($num) must be of type string|int|float, NumberFormatter given
