--TEST--
Bug #72038 (Function calls with values to a by-ref parameter don't always throw a notice)
--FILE--
<?php

try {
    test($foo = new stdClass);
    var_dump($foo);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    test($bar = 2);
    var_dump($bar);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
try {
    test($baz = &$bar);
    var_dump($baz);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

function test(&$param) {
        $param = 1;
}

?>
--EXPECT--
Exception: Cannot pass parameter 1 by reference
Exception: Cannot pass parameter 1 by reference
int(1)
