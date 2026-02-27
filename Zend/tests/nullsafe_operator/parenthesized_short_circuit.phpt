--TEST--
Parentheses should contain nullsafe short-circuiting
--FILE--
<?php

class Foo {
    public function bar(): array {
        return [1, 2, 3];
    }

    public string $prop = 'value';
}

$nullable = null;
$foo = new Foo();

echo "Without parentheses (short-circuits):\n";
var_dump($nullable?->bar()[0]);
var_dump($nullable?->prop[0]);
var_dump($foo?->bar()[0]);

echo "\nWith parentheses (should warn on null):\n";
var_dump(($nullable?->bar())[0]);
var_dump(($nullable?->prop)[0]);

echo "\nDirect null (should warn):\n";
var_dump((null)[0]);

echo "\nNon-null with parentheses:\n";
var_dump(($foo?->bar())[0]);
var_dump(($foo?->prop)[0]);

?>
--EXPECTF--
Without parentheses (short-circuits):
NULL
NULL
int(1)

With parentheses (should warn on null):

Warning: Trying to access array offset on null in %s on line %d
NULL

Warning: Trying to access array offset on null in %s on line %d
NULL

Direct null (should warn):

Warning: Trying to access array offset on null in %s on line %d
NULL

Non-null with parentheses:
int(1)
string(1) "v"
