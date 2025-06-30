--TEST--
Pipe operator respects types
--FILE--
<?php

function _test(int $a, int $b) {
    return $a + $b;
}

try {
    $res1 = "Hello" |> '_test';
    var_dump($res1);
}
catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
TypeError: _test(): Argument #1 ($a) must be of type int, string given, called in %s on line %d
