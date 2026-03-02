--TEST--
array_unique() throws ValueError on invalid sort_type
--FILE--
<?php
try {
    array_unique([1, 2, 3], 999);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    array_unique([1, 2, 3], -1);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
array_unique(): Argument #2 ($flags) must be one of SORT_REGULAR, SORT_NUMERIC, SORT_STRING, or SORT_LOCALE_STRING
array_unique(): Argument #2 ($flags) must be one of SORT_REGULAR, SORT_NUMERIC, SORT_STRING, or SORT_LOCALE_STRING

