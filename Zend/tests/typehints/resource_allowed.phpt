--TEST--
resource typehint: acceptable cases
--FILE--
<?php

function foo(resource $foo) {
    // okay
}

foo(fopen('php://memory', 'w'));

echo "OK!", PHP_EOL;
--EXPECT--
OK!

