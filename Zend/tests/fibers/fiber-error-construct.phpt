--TEST--
FiberError cannot be constructed in user code
--FILE--
<?php

try {
    new FiberError;
} catch (Error $exception) {
    echo $exception->getMessage(), "\n";
}

?>
--EXPECT--
The "FiberError" class is reserved for internal use and cannot be manually instantiated
