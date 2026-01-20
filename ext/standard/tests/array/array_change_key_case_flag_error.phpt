--TEST--
array_change_key_case(): invalid $case argument
--FILE--
<?php

try {
    array_change_key_case([], -10);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: array_change_key_case(): Argument #2 ($case) must be either CASE_LOWER or CASE_UPPER
