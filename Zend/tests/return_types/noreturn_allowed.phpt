--TEST--
noreturn return type: acceptable cases
--FILE--
<?php

function foo(): noreturn {
    throw new Exception('bad');
}

try {
    foo();
} catch (Exception $e) {
    // do nothing
}

function calls_foo(): noreturn {
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
