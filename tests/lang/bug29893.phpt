--TEST--
Bug #29893 (segfault when using array as index)
--FILE--
<?php
$base = 50;
try {
    $base[$base] -= 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot use a scalar value as an array
