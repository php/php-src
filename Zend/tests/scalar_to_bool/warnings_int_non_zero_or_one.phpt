--TEST--
Implicit int to bool conversions with values other than 0 or 1 should warn for variables
--FILE--
<?php

echo 'Function calls:' . \PHP_EOL;
function foo(bool $a) {
    return $a;
}
var_dump(foo(5));

echo 'Function returns:' . \PHP_EOL;
function bar(): bool {
    return 3;
}
var_dump(bar());

echo 'Typed property assignment:' . \PHP_EOL;
class Test {
    public bool $a;
}

$instance = new Test();
$instance->a = 13;
var_dump($instance->a);

?>
--EXPECTF--
Function calls:

Deprecated: Implicit conversion from int 5 to true, only 0 or 1 are allowed in %s on line %d
bool(true)
Function returns:

Deprecated: Implicit conversion from int 3 to true, only 0 or 1 are allowed in %s on line %d
bool(true)
Typed property assignment:

Deprecated: Implicit conversion from int 13 to true, only 0 or 1 are allowed in %s on line %d
bool(true)
