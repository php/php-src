--TEST--
FiberError cannot be constructed in user code
--FILE--
<?php

try {
    new FiberError;
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
Error: The "FiberError" class is reserved for internal use and cannot be manually instantiated
