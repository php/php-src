--TEST--
Bug #74084 (Out of bound read - zend_mm_alloc_small)
--INI--
error_reporting=0
--FILE--
<?php
$$A += $$B['a'] = &$$C;
unset($$A);
try {
    $$A -= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
unset($$A);
try {
    $$A *= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
unset($$A);
try {
    $$A /= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
unset($$A);
try {
    $$A **= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Unsupported operand types: array - array
TypeError: Unsupported operand types: array * array
TypeError: Unsupported operand types: array / array
TypeError: Unsupported operand types: array ** array
