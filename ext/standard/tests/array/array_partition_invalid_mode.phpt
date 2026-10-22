--TEST--
Test array_partition() function : invalid mode
--FILE--
<?php

foreach ([999, -1, 4] as $mode) {
    try {
        var_dump(array_partition([1, 2], fn($v) => true, $mode));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "*** Mode is validated even for an empty array, like array_filter ***\n";
try {
    var_dump(array_partition([], fn($v) => true, mode: 999));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: array_partition(): Argument #3 ($mode) must be one of ARRAY_FILTER_USE_VALUE, ARRAY_FILTER_USE_KEY, or ARRAY_FILTER_USE_BOTH
ValueError: array_partition(): Argument #3 ($mode) must be one of ARRAY_FILTER_USE_VALUE, ARRAY_FILTER_USE_KEY, or ARRAY_FILTER_USE_BOTH
ValueError: array_partition(): Argument #3 ($mode) must be one of ARRAY_FILTER_USE_VALUE, ARRAY_FILTER_USE_KEY, or ARRAY_FILTER_USE_BOTH
*** Mode is validated even for an empty array, like array_filter ***
ValueError: array_partition(): Argument #3 ($mode) must be one of ARRAY_FILTER_USE_VALUE, ARRAY_FILTER_USE_KEY, or ARRAY_FILTER_USE_BOTH
