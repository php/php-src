--TEST--
Call internal function with incorrect number of arguments with strict types
--FILE--
<?php
declare(strict_types=1);
try {
    substr("foo");
} catch (ArgumentCountError $e) {
    echo get_class($e) . PHP_EOL;
    echo $e->getMessage() . PHP_EOL;
}

try {
    array_diff([]);
} catch (ArgumentCountError $e) {
    echo get_class($e) . PHP_EOL;
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ArgumentCountError
substr(): At least 2 arguments are expected, 1 given
ArgumentCountError
At least 2 parameters are required, 1 given
