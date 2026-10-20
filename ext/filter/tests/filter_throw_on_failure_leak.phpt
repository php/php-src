--TEST--
filter: FILTER_THROW_ON_FAILURE does not leak the preserved input string
--EXTENSIONS--
filter
--FILE--
<?php
// php_zval_filter() copies the input string so it can be quoted in the
// exception message. A non-string scalar input (here a float / a large int)
// is turned into a fresh heap string by convert_to_string(), so the copy is
// the sole extra owner. Releasing it with zend_string_delref() decremented
// without freeing, leaking one string per call on both the failure and the
// success path. Loop and assert memory stays flat.
function leakcheck(callable $fn): bool {
    $fn();
    $before = memory_get_usage();
    for ($i = 0; $i < 2000; $i++) {
        $fn();
    }
    return memory_get_usage() - $before === 0;
}

// Validation fails -> exception thrown.
var_dump(leakcheck(function () {
    try {
        filter_var(1.5, FILTER_VALIDATE_INT, ['flags' => FILTER_THROW_ON_FAILURE]);
    } catch (\Filter\FilterFailedException $e) {
    }
}));

// Validation succeeds.
var_dump(leakcheck(function () {
    filter_var(15, FILTER_VALIDATE_INT, ['flags' => FILTER_THROW_ON_FAILURE]);
}));
?>
--EXPECT--
bool(true)
bool(true)
