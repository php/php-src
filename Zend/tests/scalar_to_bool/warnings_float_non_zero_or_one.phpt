--TEST--
Implicit float to bool conversions with values other than 0 or 1 should warn for variables
--FILE--
<?php

echo 'Function calls:' . \PHP_EOL;
function foo(bool $a) {
    return $a;
}
var_dump(foo(1.1));

echo 'Function returns:' . \PHP_EOL;
function bar(): bool {
    return -0.3;
}
var_dump(bar());

echo 'Typed property assignment:' . \PHP_EOL;
class Test {
    public bool $a;
}

$instance = new Test();
$instance->a = 13.0;
var_dump($instance->a);

?>
--EXPECTF--
Function calls:

Deprecated: Implicit conversion from float 1.1 to true, only 0 or 1 are allowed in %s on line %d
bool(true)
Function returns:

Deprecated: Implicit conversion from float -0.3 to true, only 0 or 1 are allowed in %s on line %d
bool(true)
Typed property assignment:

Deprecated: Implicit conversion from float 13 to true, only 0 or 1 are allowed in %s on line %d
bool(true)
