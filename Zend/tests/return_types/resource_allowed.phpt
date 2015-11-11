--TEST--
resource return type: acceptable cases
--FILE--
<?php

function bar(): resource {
    return fopen('php://memory', 'w'); // okay
}

bar();

echo "OK!", PHP_EOL;
--EXPECT--
OK!

