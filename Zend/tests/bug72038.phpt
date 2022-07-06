--TEST--
Bug #72038 (Function calls with values to a by-ref parameter don't always throw a notice)
--FILE--
<?php

try {
    test($foo = new stdClass);
    var_dump($foo);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
try {
    test($bar = 2);
    var_dump($bar);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

test($baz = &$bar);
var_dump($baz);

function test(&$param) {
        $param = 1;
}

?>
--EXPECT--
test(): Argument #1 ($param) cannot be passed by reference
test(): Argument #1 ($param) cannot be passed by reference
int(1)
