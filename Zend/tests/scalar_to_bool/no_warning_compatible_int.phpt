--TEST--
Compatible int to bool conversions with values 0 or 1
--FILE--
<?php

echo 'Function calls:' . \PHP_EOL;
function foo(bool $a) {
    return $a;
}
var_dump(foo(1));
var_dump(foo(0));
var_dump(foo(-0));

echo 'Function returns:' . \PHP_EOL;
function bar(): bool {
    return 0;
}
var_dump(bar());

echo 'Typed property assignment:' . \PHP_EOL;
class Test {
    public bool $a;
}

$instance = new Test();
$instance->a = 1;
var_dump($instance->a);

?>
--EXPECTF--
Function calls:
bool(true)
bool(false)
bool(false)
Function returns:
bool(false)
Typed property assignment:
bool(true)
