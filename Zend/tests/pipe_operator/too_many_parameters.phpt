--TEST--
Pipe operator fails on multi-parameter functions
--FILE--
<?php

function _test(int $a, int $b) {
    return $a + $b;
}


try {
    $res1 = 5 |> '_test';
}
catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}


?>
--EXPECTF--
ArgumentCountError: Too few arguments to function %s, 1 passed in %s on line %s and exactly 2 expected
