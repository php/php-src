--TEST--
Bug #36988 (mktime freezes on long numbers)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
date_default_timezone_set('GMT');
$start = microtime(true);
$a = mktime(1, 1, 1, 1, 1, 11111111111);
?>
--EXPECTF--
Warning: mktime() expects parameter 6 to be int, float given in %s on line %d
