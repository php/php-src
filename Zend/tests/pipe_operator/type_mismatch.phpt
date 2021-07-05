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
    printf("Expected %s thrown, got %s", TypeError::class, get_class($e));
}

?>
--EXPECT--
Expected TypeError thrown, got TypeError
