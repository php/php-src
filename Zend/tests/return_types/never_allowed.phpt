--TEST--
never return type: acceptable cases
--FILE--
<?php

function foo(): never {
    throw new Exception('bad');
}

try {
    foo();
} catch (Exception $e) {
    // do nothing
}

function calls_foo(): never {
    foo();
}

try {
    calls_foo();
} catch (Exception $e) {
    // do nothing
}

echo "OK!", PHP_EOL;
?>
--EXPECT--
OK!
