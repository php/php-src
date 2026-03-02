--TEST--
Bug #36988 (mktime freezes on long numbers)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
date_default_timezone_set('GMT');
$start = microtime(true);
try {
    $a = mktime(1, 1, 1, 1, 1, 11111111111);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: mktime(): Argument #6 ($year) must be of type ?int, float given
