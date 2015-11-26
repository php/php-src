--TEST--
Test log10 function : 64bit long tests
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

$longVals = array(
    MAX_64Bit, MIN_64Bit, MAX_32Bit, MIN_32Bit, MAX_64Bit - MAX_32Bit, MIN_64Bit - MIN_32Bit,
    MAX_32Bit + 1, MIN_32Bit - 1, MAX_32Bit * 2, (MAX_32Bit * 2) + 1, (MAX_32Bit * 2) - 1,
    MAX_64Bit -1, MAX_64Bit + 1, MIN_64Bit + 1, MIN_64Bit - 1
);


foreach ($longVals as $longVal) {
   echo "--- testing: $longVal ---\n";
   var_dump(log10($longVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
float(18.964889726831)
--- testing: -9223372036854775808 ---
float(NAN)
--- testing: 2147483647 ---
float(9.3319298653812)
--- testing: -2147483648 ---
float(NAN)
--- testing: 9223372034707292160 ---
float(18.96488972673)
--- testing: -9223372034707292160 ---
float(NAN)
--- testing: 2147483648 ---
float(9.3319298655834)
--- testing: -2147483649 ---
float(NAN)
--- testing: 4294967294 ---
float(9.6329598610452)
--- testing: 4294967295 ---
float(9.6329598611463)
--- testing: 4294967293 ---
float(9.632959860944)
--- testing: 9223372036854775806 ---
float(18.964889726831)
--- testing: 9.2233720368548E+18 ---
float(18.964889726831)
--- testing: -9223372036854775807 ---
float(NAN)
--- testing: -9.2233720368548E+18 ---
float(NAN)
===DONE===
