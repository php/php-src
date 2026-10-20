--TEST--
PFA receives by ref if the actual function does
--FILE--
<?php

function foo($a, &$b) {
    $a = 2;
}

$foo = foo(1, ?);

try {
    $foo(2);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
{closure:%s}(): Argument #1 ($b) could not be passed by reference
