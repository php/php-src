--TEST--
Silencing a userland function argument emitting a diagnostic
--FILE--
<?php

function test1($arg) {
    return $arg;
}

$var = test1(@$non_existent);

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
