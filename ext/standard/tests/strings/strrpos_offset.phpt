--TEST--
strrpos() offset integer overflow
--FILE--
<?php

try {
    var_dump(strrpos("t", "t", PHP_INT_MAX+1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    strrpos(1024, 1024, -PHP_INT_MAX);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    strrpos(1024, "te", -PHP_INT_MAX);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    strrpos(1024, 1024, -PHP_INT_MAX-1);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    strrpos(1024, "te", -PHP_INT_MAX-1);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
TypeError: strrpos(): Argument #3 ($offset) must be of type int, float given
ValueError: strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
ValueError: strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
Done
