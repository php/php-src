--TEST--
GH-19899 (undefined behavior with zend_strtod with large digits as strings)
--INI--
memory_limit=-1
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
(int)str_repeat("1", 2147483648);
echo "OK";
?>
--EXPECT--
OK
