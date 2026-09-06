--TEST--
strripos() offset integer overflow
--FILE--
<?php

try {
    var_dump(strripos("t", "t", PHP_INT_MAX+1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    strripos(1024, 1024, -PHP_INT_MAX);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    strripos(1024, "te", -PHP_INT_MAX);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    strripos(1024, 1024, -PHP_INT_MAX-1);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    strripos(1024, "te", -PHP_INT_MAX-1);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
TypeError: strripos(): Argument #3 ($offset) must be of type int, float given
ValueError: strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
Done
