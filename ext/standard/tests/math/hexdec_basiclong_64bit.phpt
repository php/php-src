--TEST--
Test hexdec function : 64bit long tests
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

define("MAX_64Bit", 9223372036854775807);
define("MAX_32Bit", 2147483647);
define("MIN_64Bit", -9223372036854775807 - 1);
define("MIN_32Bit", -2147483647 - 1);

$hexLongStrs = array(
   '7'.str_repeat('f',15),
   str_repeat('f',16),
   '7'.str_repeat('f',7),
   str_repeat('f',8),
   '7'.str_repeat('f',16),
   str_repeat('f',18),
   '7'.str_repeat('f',8),
   str_repeat('f',9)
);


foreach ($hexLongStrs as $strVal) {
   echo "--- testing: $strVal ---\n";
   var_dump(hexdec($strVal));
}

?>
--EXPECT--
--- testing: 7fffffffffffffff ---
int(9223372036854775807)
--- testing: ffffffffffffffff ---
float(1.8446744073709552E+19)
--- testing: 7fffffff ---
int(2147483647)
--- testing: ffffffff ---
int(4294967295)
--- testing: 7ffffffffffffffff ---
float(1.4757395258967641E+20)
--- testing: ffffffffffffffffff ---
float(4.722366482869645E+21)
--- testing: 7ffffffff ---
int(34359738367)
--- testing: fffffffff ---
int(68719476735)
