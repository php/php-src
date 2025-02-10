--TEST--
Pipe binds higher than comparison
--FILE--
<?php

function _test1(int $a): int {
    return $a * 2;
}

$res1 = (5 |> _test1(...)) == 10 ;
var_dump($res1);

// This will error without the parens,
// as it will try to compare a closure to an int first.
try {
    $res1 = 5 |> _test1(...) == 10 ;
}
catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
bool(true)

Notice: Object of class Closure could not be converted to int in %s line %d
Error: Value of type bool is not callable
