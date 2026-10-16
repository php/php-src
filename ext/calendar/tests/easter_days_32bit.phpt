--TEST--
Test easter_days() year range on 32bit systems (GH-16228)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip 32-bit only"); ?>
--EXTENSIONS--
calendar
--FILE--
<?php
$max = intdiv(PHP_INT_MAX, 5) * 4;
foreach ([PHP_INT_MIN, -1, 0, 1, $max - 1, $max, $max + 1, PHP_INT_MAX] as $year) {
    echo $year, ': ';
    try {
        var_dump(easter_days($year));
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
-2147483648: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 1717986916
-1: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 1717986916
0: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 1717986916
1: int(6)
1717986915: int(24)
1717986916: int(15)
1717986917: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 1717986916
2147483647: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 1717986916
