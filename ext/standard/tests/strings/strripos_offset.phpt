--TEST--
strripos() offset integer overflow
--FILE--
<?php

try {
    var_dump(strripos("t", "t", PHP_INT_MAX+1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    strripos(1024, 1024, -PHP_INT_MAX);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos(1024, "te", -PHP_INT_MAX);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos(1024, 1024, -PHP_INT_MAX-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    strripos(1024, "te", -PHP_INT_MAX-1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
strripos(): Argument #3 ($offset) must be of type int, float given
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
Done
