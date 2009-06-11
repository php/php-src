--TEST--
Test sin function : 64bit long tests
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
   var_dump(sin($longVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
float(0.9999303767)
--- testing: -9223372036854775808 ---
float(-0.9999303767)
--- testing: 2147483647 ---
float(-0.7249165551)
--- testing: -2147483648 ---
float(0.9713101758)
--- testing: 9223372034707292160 ---
float(0.2492611714)
--- testing: -9223372034707292160 ---
float(-0.2492611714)
--- testing: 2147483648 ---
float(-0.9713101758)
--- testing: -2147483649 ---
float(0.3246857002)
--- testing: 4294967294 ---
float(0.9986982435)
--- testing: 4294967295 ---
float(0.4966771918)
--- testing: 4294967293 ---
float(0.5825207359)
--- testing: 9223372036854775806 ---
float(0.9999303767)
--- testing: 9.223372037E+18 ---
float(0.9999303767)
--- testing: -9223372036854775807 ---
float(-0.9999303767)
--- testing: -9.223372037E+18 ---
float(-0.9999303767)
===DONE===