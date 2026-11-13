--TEST--
filter_var_array() does not leak the filtered value when filtering it throws
--EXTENSIONS--
filter
--FILE--
<?php
try {
    filter_var_array(['a' => ['1', 'x']], ['a' => ['filter' => FILTER_VALIDATE_INT, 'flags' => FILTER_REQUIRE_ARRAY | FILTER_THROW_ON_FAILURE]]);
} catch (Filter\FilterFailedException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    filter_var_array(['a' => ['1', 'x']], ['a' => ['filter' => FILTER_CALLBACK, 'options' => function ($value) {
        if ($value === 'x') {
            throw new Exception('callback failed');
        }
        return $value;
    }]]);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Filter\FilterFailedException: filter validation failed: filter int not satisfied by 'x'
Exception: callback failed
