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
    printf("Expected %s thrown, got %s", ArgumentCountError::class, get_class($e));
}


?>
--EXPECT--
Expected ArgumentCountError thrown, got ArgumentCountError
