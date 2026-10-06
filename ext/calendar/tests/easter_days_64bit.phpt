--TEST--
Test easter_days() year range on 64bit systems (GH-16228)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
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
-9223372036854775808: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 7378697629483820644
-1: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 7378697629483820644
0: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 7378697629483820644
1: int(6)
7378697629483820643: int(19)
7378697629483820644: int(3)
7378697629483820645: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 7378697629483820644
9223372036854775807: ValueError: easter_days(): Argument #1 ($year) must be between 1 and 7378697629483820644
