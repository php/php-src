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
    echo $e->getMessage(), "\n";
}
unset($$A);
try {
    $$A *= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
unset($$A);
try {
    $$A /= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
unset($$A);
try {
    $$A **= $$B['a'] = &$$C;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Unsupported operand types: array - array
Unsupported operand types: array * array
Unsupported operand types: array / array
Unsupported operand types: array ** array
