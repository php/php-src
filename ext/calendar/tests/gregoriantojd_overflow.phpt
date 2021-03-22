--TEST--
gregoriantojd()
--EXTENSIONS--
calendar
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
echo gregoriantojd(5, 5, 6000000) . "\n";
?>
--EXPECT--
2193176185
