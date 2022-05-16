--TEST--
Implicit string to bool conversions with values other than empty string, "0" or "1" should warn for variables
--FILE--
<?php

echo 'Function calls:' . \PHP_EOL;
function foo(bool $a) {
    return $a;
}
var_dump(foo("on"));

echo 'Function returns:' . \PHP_EOL;
function bar(): bool {
    return "inactive";
}
var_dump(bar());

echo 'Typed property assignment:' . \PHP_EOL;
class Test {
    public bool $a;
}

$instance = new Test();
$instance->a = "0.0";
var_dump($instance->a);

?>
--EXPECTF--
Function calls:

Deprecated: Implicit conversion from string "on" to true, only "", "0" or "1" are allowed in %s on line %d
bool(true)
Function returns:

Deprecated: Implicit conversion from string "inactive" to true, only "", "0" or "1" are allowed in %s on line %d
bool(true)
Typed property assignment:

Deprecated: Implicit conversion from string "0.0" to true, only "", "0" or "1" are allowed in %s on line %d
bool(true)
