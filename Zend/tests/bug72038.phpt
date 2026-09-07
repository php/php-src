--TEST--
Bug #72038 (Function calls with values to a by-ref parameter don't always throw a notice)
--FILE--
<?php

try {
    test($foo = new stdClass);
    var_dump($foo);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    test($bar = 2);
    var_dump($bar);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

test($baz = &$bar);
var_dump($baz);

function test(&$param) {
        $param = 1;
}

?>
--EXPECT--
Error: test(): Argument #1 ($param) could not be passed by reference
Error: test(): Argument #1 ($param) could not be passed by reference
int(1)
