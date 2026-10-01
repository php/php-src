--TEST--
Bug #29893 (segfault when using array as index)
--FILE--
<?php
$base = 50;
try {
    $base[$base] -= 0;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot use a scalar value as an array
