--TEST--
juliantojd()
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo juliantojd(5, 5, 6000000000) . "\n";
?>
--EXPECT--
622764916319
