--TEST--
Test bindec function : 64bit long tests
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

$binLongStrs = array(
   '0'.str_repeat('1',63), 
   str_repeat('1',64), 
   '0'.str_repeat('1',31), 
   str_repeat('1',32), 
   '0'.str_repeat('1',64),
   str_repeat('1',65), 
   '0'.str_repeat('1',32), 
   str_repeat('1',33)
);


foreach ($binLongStrs as $strVal) {
   echo "--- testing: $strVal ---\n";
   var_dump(bindec($strVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 0111111111111111111111111111111111111111111111111111111111111111 ---
int(9223372036854775807)
--- testing: 1111111111111111111111111111111111111111111111111111111111111111 ---
float(1.844674407371E+19)
--- testing: 01111111111111111111111111111111 ---
int(2147483647)
--- testing: 11111111111111111111111111111111 ---
int(4294967295)
--- testing: 01111111111111111111111111111111111111111111111111111111111111111 ---
float(1.844674407371E+19)
--- testing: 11111111111111111111111111111111111111111111111111111111111111111 ---
float(3.6893488147419E+19)
--- testing: 011111111111111111111111111111111 ---
int(4294967295)
--- testing: 111111111111111111111111111111111 ---
int(8589934591)
===DONE===
