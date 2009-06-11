--TEST--
Test tan function : 64bit long tests
--INI--
precision=10
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
   var_dump(tan($longVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
float(84.73931297)
--- testing: -9223372036854775808 ---
float(-84.73931297)
--- testing: 2147483647 ---
float(1.052377964)
--- testing: -2147483648 ---
float(4.084289455)
--- testing: 9223372034707292160 ---
float(-0.2573852005)
--- testing: -9223372034707292160 ---
float(0.2573852005)
--- testing: 2147483648 ---
float(-4.084289455)
--- testing: -2147483649 ---
float(0.3432841603)
--- testing: 4294967294 ---
float(-19.57923809)
--- testing: 4294967295 ---
float(-0.5722513702)
--- testing: 4294967293 ---
float(0.7166700082)
--- testing: 9223372036854775806 ---
float(84.73931297)
--- testing: 9.223372037E+18 ---
float(84.73931297)
--- testing: -9223372036854775807 ---
float(-84.73931297)
--- testing: -9.223372037E+18 ---
float(-84.73931297)
===DONE===