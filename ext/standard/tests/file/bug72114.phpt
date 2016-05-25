--TEST--
Bug #72114 (Integer underflow / arbitrary null write in fread/gzread)
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
ini_set('memory_limit', "2500M");
$fp = fopen("/dev/zero", "r");
fread($fp, 2147483648);
?>
Done
--EXPECTF--
Warning: fread(): Length parameter must be no more than 2147483647 in %s%ebug72114.php on line %d
Done
