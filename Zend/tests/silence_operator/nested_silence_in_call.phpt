--TEST--
Nested at in call must not fail
--FILE--
<?php

function foo($arg) {
}

function bar() {
    throw new Exception("test");
}

try {
    $var = @foo(@bar());
} catch (\Throwable $e) {
    echo $e::class, \PHP_EOL;
}

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
