--TEST--
juliantojd()
--EXTENSIONS--
calendar
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* The largest year JulianToSdn() accepts, INT_MAX - 4800. */
echo juliantojd(5, 5, 2147478847) . "\n";
/* Anything above it is rejected rather than truncated. */
echo juliantojd(5, 5, 6000000000) . "\n";
?>
--EXPECT--
784368370049
0
