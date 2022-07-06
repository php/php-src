--TEST--
Bug #77660 (Segmentation fault on break 2147483648)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
for(;;) break 2147483648;
?>
--EXPECTF--
Fatal error: Cannot 'break' 2147483648 levels in %sbug77660.php on line %d
