--TEST--
Silencing a userland function argument throwing an Exception
--FILE--
<?php

function test1($arg) {
    return $arg;
}

function foo() {
    throw new Exception();
    return 1;
}

$var = test1(@foo());

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
