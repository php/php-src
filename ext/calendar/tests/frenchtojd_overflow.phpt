--TEST--
frenchtojd(): out-of-range arguments that alias into valid range via 32-bit narrowing must return 0
--EXTENSIONS--
calendar
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64-bit only");
?>
--FILE--
<?php
// Values whose low 32 bits land in the valid range must still be rejected.
// 2**32 == 4294967296; adding it to a valid value produces an alias.
$bias = 2 ** 32;

// month out of range (aliases to 1)
var_dump(frenchtojd(1 + $bias, 1, 1));   // month 4294967297 -> 0
// day out of range (aliases to 1)
var_dump(frenchtojd(1, 1 + $bias, 1));   // day 4294967297 -> 0
// year out of range (aliases to 1)
var_dump(frenchtojd(1, 1, 1 + $bias));   // year 4294967297 -> 0

// Valid call still works
var_dump(frenchtojd(1, 1, 1));            // int(2375840)
?>
--EXPECT--
int(0)
int(0)
int(0)
int(2375840)
