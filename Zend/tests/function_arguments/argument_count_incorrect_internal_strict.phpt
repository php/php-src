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
    array_diff();
} catch (ArgumentCountError $e) {
    echo get_class($e) . PHP_EOL;
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ArgumentCountError
substr() expects at least 2 arguments, 1 given
ArgumentCountError
array_diff() expects at least 1 argument, 0 given
